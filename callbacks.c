#include <gtk/gtk.h>
#include <glade/glade.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "callbacks.h"

extern GladeXML *xml;

#define FILE_MEMBRES "membre.txt"

/* ===================== Helpers UI ===================== */
static GtkWidget* W(const char *name) {
    return (xml && name) ? glade_xml_get_widget(xml, name) : NULL;
}

static GtkWidget* WIN_ADMIN(void) {
    GtkWidget *w = W("admin gestion");
    if (!w) w = W("window1");
    return w;
}

/* ✅ robuste: la vraie fenêtre fiche via entry1 */
static GtkWidget* WIN_FICHE(void)
{
    GtkWidget *e = W("entry1"); /* champ Nom */
    if (e) {
        GtkWidget *top = gtk_widget_get_toplevel(e);
        if (top && GTK_IS_WINDOW(top)) return top;
    }
    GtkWidget *w = W("window3");
    if (w && GTK_IS_WINDOW(w)) return w;
    return NULL;
}

static void popup(GtkMessageType type, const char *msg) {
    GtkWidget *parent = WIN_ADMIN();
    if (!parent || !GTK_IS_WINDOW(parent)) parent = NULL;
    GtkWidget *d = gtk_message_dialog_new(parent ? GTK_WINDOW(parent) : NULL,
                                          GTK_DIALOG_MODAL,
                                          type, GTK_BUTTONS_OK,
                                          "%s", msg ? msg : "");
    gtk_dialog_run(GTK_DIALOG(d));
    gtk_widget_destroy(d);
}

static int confirm_yesno(const char *msg) {
    GtkWidget *parent = WIN_ADMIN();
    if (!parent || !GTK_IS_WINDOW(parent)) parent = NULL;
    GtkWidget *d = gtk_message_dialog_new(parent ? GTK_WINDOW(parent) : NULL,
                                          GTK_DIALOG_MODAL,
                                          GTK_MESSAGE_QUESTION,
                                          GTK_BUTTONS_YES_NO,
                                          "%s", msg ? msg : "");
    int r = gtk_dialog_run(GTK_DIALOG(d));
    gtk_widget_destroy(d);
    return (r == GTK_RESPONSE_YES);
}

static const char* entry_get(const char *id) {
    GtkWidget *e = W(id);
    return (e && GTK_IS_ENTRY(e)) ? gtk_entry_get_text(GTK_ENTRY(e)) : "";
}
static void entry_set(const char *id, const char *v) {
    GtkWidget *e = W(id);
    if (e && GTK_IS_ENTRY(e)) gtk_entry_set_text(GTK_ENTRY(e), v ? v : "");
}

static int spin_get(const char *id, int defv) {
    GtkWidget *s = W(id);
    return (s && GTK_IS_SPIN_BUTTON(s)) ? gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(s)) : defv;
}
static void spin_set(const char *id, int v) {
    GtkWidget *s = W(id);
    if (s && GTK_IS_SPIN_BUTTON(s)) gtk_spin_button_set_value(GTK_SPIN_BUTTON(s), v);
}

/* ===================== TreeView actif (si notebook/onglets) ===================== */
static void collect_treeviews(GtkWidget *w, GPtrArray *arr)
{
    if (!w) return;

    if (GTK_IS_TREE_VIEW(w)) g_ptr_array_add(arr, w);

    if (GTK_IS_CONTAINER(w)) {
        GList *children = gtk_container_get_children(GTK_CONTAINER(w));
        for (GList *l = children; l; l = l->next)
            collect_treeviews(GTK_WIDGET(l->data), arr);
        g_list_free(children);
    }
}

static GtkWidget* TREE_ACTIVE_IN_ADMIN(void)
{
    /* 1) treeview1 direct (meilleur cas) */
    GtkWidget *t = W("treeview1");
    if (t && GTK_IS_TREE_VIEW(t)) return t;

    /* 2) focus */
    GtkWidget *admin = WIN_ADMIN();
    if (admin && GTK_IS_WINDOW(admin)) {
        GtkWidget *focus = gtk_window_get_focus(GTK_WINDOW(admin));
        if (focus && GTK_IS_TREE_VIEW(focus)) return focus;
    }

    /* 3) premier treeview visible */
    if (!admin) return NULL;

    GPtrArray *arr = g_ptr_array_new();
    collect_treeviews(admin, arr);

    GtkWidget *best = NULL;
    for (guint i = 0; i < arr->len; i++) {
        GtkWidget *tv = g_ptr_array_index(arr, i);
        if (GTK_WIDGET_VISIBLE(tv)) { best = tv; break; } /* GTK2 */
    }

    g_ptr_array_free(arr, TRUE);
    return best;
}

/* ===================== Validation Téléphone (entry7) ===================== */
static int is_all_digits(const char *s)
{
    if (!s || !*s) return 0;
    for (const unsigned char *p = (const unsigned char*)s; *p; ++p) {
        if (!isdigit(*p)) return 0;
    }
    return 1;
}

/* Tunisie: 8 chiffres */
static int validate_tel_or_popup(const char *tel)
{
    if (!tel || !*tel) {
        popup(GTK_MESSAGE_ERROR, "Numéro de téléphone obligatoire.");
        return 0;
    }
    if (!is_all_digits(tel)) {
        popup(GTK_MESSAGE_ERROR, "Numéro invalide : seulement des chiffres (0-9).");
        return 0;
    }
    if ((int)strlen(tel) != 8) {
        popup(GTK_MESSAGE_ERROR, "Numéro invalide : il doit contenir exactement 8 chiffres.");
        return 0;
    }
    return 1;
}

/* Optionnel: bloque lettres pendant la saisie (si signal insert-text connecté dans Glade) */
void on_entry7_insert_text(GtkEditable *editable, const gchar *text, gint length, gint *position, gpointer data)
{
    (void)position; (void)data;
    for (int i = 0; i < length; i++) {
        if (!g_ascii_isdigit(text[i])) {
            g_signal_stop_emission_by_name(editable, "insert-text");
            return;
        }
    }
}

/* ===================== CSV helpers ===================== */
static int split_tokens(const char *line, char ***out) {
    char *tmp = g_strdup(line);
    g_strchomp(tmp);
    char **toks = g_strsplit(tmp, ";", -1);
    g_free(tmp);
    int n=0; while (toks && toks[n]) n++;
    *out = toks;
    return n;
}
static const char* tok(char **toks, int n, int i) {
    return (i<n && toks[i]) ? toks[i] : "";
}

static int strcasestr_simple(const char *hay, const char *nee) {
    if (!hay || !nee) return 0;
    if (!*nee) return 1;
    for (const char *p = hay; *p; p++) {
        const char *a=p, *b=nee;
        while (*a && *b && tolower((unsigned char)*a)==tolower((unsigned char)*b)) { a++; b++; }
        if (!*b) return 1;
    }
    return 0;
}

/* ===================== TreeView columns/model ===================== */
enum {
    COL_ID,
    COL_NOM,
    COL_PRENOM,
    COL_DATE,
    COL_SEXE,
    COL_EMAIL,
    COL_TEL,
    COL_ADR,
    N_COLS
};

static void init_cols(GtkWidget *tree) {
    if (!tree || !GTK_IS_TREE_VIEW(tree)) return;
    GtkTreeView *tv = GTK_TREE_VIEW(tree);

    /* GTK2: vérifier si déjà colonnes */
    GList *cols = gtk_tree_view_get_columns(tv);
    if (cols) { g_list_free(cols); return; }

    GtkCellRenderer *r;

    r = gtk_cell_renderer_text_new();
    gtk_tree_view_append_column(tv, gtk_tree_view_column_new_with_attributes("ID", r, "text", COL_ID, NULL));

    r = gtk_cell_renderer_text_new();
    gtk_tree_view_append_column(tv, gtk_tree_view_column_new_with_attributes("Nom", r, "text", COL_NOM, NULL));

    r = gtk_cell_renderer_text_new();
    gtk_tree_view_append_column(tv, gtk_tree_view_column_new_with_attributes("Prénom", r, "text", COL_PRENOM, NULL));

    r = gtk_cell_renderer_text_new();
    gtk_tree_view_append_column(tv, gtk_tree_view_column_new_with_attributes("Date", r, "text", COL_DATE, NULL));

    r = gtk_cell_renderer_text_new();
    gtk_tree_view_append_column(tv, gtk_tree_view_column_new_with_attributes("Sexe", r, "text", COL_SEXE, NULL));

    r = gtk_cell_renderer_text_new();
    gtk_tree_view_append_column(tv, gtk_tree_view_column_new_with_attributes("Email", r, "text", COL_EMAIL, NULL));

    r = gtk_cell_renderer_text_new();
    gtk_tree_view_append_column(tv, gtk_tree_view_column_new_with_attributes("Téléphone", r, "text", COL_TEL, NULL));

    r = gtk_cell_renderer_text_new();
    gtk_tree_view_append_column(tv, gtk_tree_view_column_new_with_attributes("Adresse", r, "text", COL_ADR, NULL));
}

static void load_tree_filtered(GtkWidget *tree, const char *needle) {
    if (!tree || !GTK_IS_TREE_VIEW(tree)) return;
    init_cols(tree);

    GtkListStore *store = gtk_list_store_new(
        N_COLS,
        G_TYPE_INT,
        G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
        G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING
    );
    gtk_tree_view_set_model(GTK_TREE_VIEW(tree), GTK_TREE_MODEL(store));

    FILE *f = fopen(FILE_MEMBRES, "r");
    if (!f) { g_object_unref(store); return; }

    char line[4096];
    while (fgets(line, sizeof line, f)) {
        char **toks=NULL; int n=split_tokens(line, &toks);
        if (n >= 8) {
            const char *nom    = tok(toks,n,1);
            const char *prenom = tok(toks,n,2);
            const char *date   = tok(toks,n,3);
            const char *sexe   = tok(toks,n,4);
            const char *email  = tok(toks,n,5);
            const char *tel    = tok(toks,n,6);
            const char *adr    = tok(toks,n,7);

            int ok = 1;
            if (needle && *needle) {
                ok = strcasestr_simple(nom, needle) ||
                     strcasestr_simple(prenom, needle) ||
                     strcasestr_simple(email, needle) ||
                     strcasestr_simple(tel, needle) ||
                     strcasestr_simple(adr, needle);
            }

            if (ok) {
                GtkTreeIter it;
                gtk_list_store_append(store, &it);
                gtk_list_store_set(store, &it,
                    COL_ID, atoi(tok(toks,n,0)),
                    COL_NOM, nom,
                    COL_PRENOM, prenom,
                    COL_DATE, date,
                    COL_SEXE, sexe,
                    COL_EMAIL, email,
                    COL_TEL, tel,
                    COL_ADR, adr,
                    -1);
            }
        }
        if (toks) g_strfreev(toks);
    }
    fclose(f);
    g_object_unref(store);
}

static void load_tree_all(GtkWidget *tree) { load_tree_filtered(tree, ""); }

/* ===================== Stats ===================== */
static int set_label_int(const char *id, int v) {
    GtkWidget *w = W(id);
    if (!w || !GTK_IS_LABEL(w)) return 0;
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", v);
    gtk_label_set_text(GTK_LABEL(w), buf);
    return 1;
}

static void update_stats_from_file(void) {
    int total=0, homme=0, femme=0;

    FILE *f = fopen(FILE_MEMBRES, "r");
    if (f) {
        char line[4096];
        while (fgets(line, sizeof line, f)) {
            char **toks=NULL; int n=split_tokens(line, &toks);
            if (n >= 5) {
                total++;
                const char *sx = tok(toks,n,4);
                if (g_ascii_strcasecmp(sx,"HOMME")==0 || g_ascii_strcasecmp(sx,"M")==0) homme++;
                else if (g_ascii_strcasecmp(sx,"FEMME")==0 || g_ascii_strcasecmp(sx,"F")==0) femme++;
            }
            if (toks) g_strfreev(toks);
        }
        fclose(f);
    }

    /* ⚠️ Mets tes IDs exacts si différents */
    set_label_int("totale900", total);
    set_label_int("homme900",  homme);
    set_label_int("femme900",  femme);
}

/* ===================== Fichier helpers ===================== */
static int get_next_id(void) {
    FILE *f = fopen(FILE_MEMBRES, "r");
    if (!f) return 1;
    int max_id=0;
    char line[4096];
    while (fgets(line, sizeof line, f)) {
        char **toks=NULL; int n=split_tokens(line, &toks);
        if (n>=1) {
            int id = atoi(tok(toks,n,0));
            if (id > max_id) max_id = id;
        }
        if (toks) g_strfreev(toks);
    }
    fclose(f);
    return max_id + 1;
}

static int update_member_by_id(int id_mod,
                               const char *nom, const char *prenom,
                               const char *date, const char *sexe,
                               const char *email, const char *tel, const char *adr)
{
    char tmpname[256];
    snprintf(tmpname, sizeof(tmpname), "%s.tmp", FILE_MEMBRES);

    FILE *fin = fopen(FILE_MEMBRES, "r");
    if (!fin) return 0;
    FILE *fout = fopen(tmpname, "w");
    if (!fout) { fclose(fin); return 0; }

    char line[4096];
    int found=0;

    while (fgets(line, sizeof line, fin)) {
        char **toks=NULL; int n=split_tokens(line, &toks);
        int id = (n>=1) ? atoi(tok(toks,n,0)) : -1;

        if (id == id_mod) {
            found=1;

            /* au moins 8 champs */
            if (n < 8) {
                char **newt = g_new0(char*, 9);
                for (int i=0;i<n;i++) newt[i]=g_strdup(tok(toks,n,i));
                for (int i=n;i<8;i++) newt[i]=g_strdup("");
                newt[8]=NULL;
                g_strfreev(toks);
                toks=newt;
                n=8;
            }

            g_free(toks[1]); toks[1]=g_strdup(nom?nom:"");
            g_free(toks[2]); toks[2]=g_strdup(prenom?prenom:"");
            g_free(toks[3]); toks[3]=g_strdup(date?date:"");
            g_free(toks[4]); toks[4]=g_strdup(sexe?sexe:"");
            g_free(toks[5]); toks[5]=g_strdup(email?email:"");
            g_free(toks[6]); toks[6]=g_strdup(tel?tel:"");
            g_free(toks[7]); toks[7]=g_strdup(adr?adr:"");

            for (int i=0; toks[i]; i++) {
                fputs(toks[i], fout);
                if (toks[i+1]) fputc(';', fout);
            }
            fputc('\n', fout);
        } else {
            g_strchomp(line);
            fputs(line, fout);
            fputc('\n', fout);
        }

        if (toks) g_strfreev(toks);
    }

    fclose(fin); fclose(fout);

    if (!found) { remove(tmpname); return 0; }
    remove(FILE_MEMBRES);
    return (rename(tmpname, FILE_MEMBRES) == 0);
}

/* ===================== Etat modification ===================== */
static int g_edit_id = -1;

/* ===================== CALLBACKS ===================== */

/* afficher */
void on_khayri300_clicked(GtkButton *button, gpointer user_data) {
    (void)button; (void)user_data;
    GtkWidget *tree = TREE_ACTIVE_IN_ADMIN();
    if (!tree) { popup(GTK_MESSAGE_ERROR,"Aucun treeview trouvé dans admin."); return; }
    load_tree_all(tree);
    update_stats_from_file();
}

/* rechercher (option: entry34) */
void on_khayri9_clicked(GtkButton *button, gpointer user_data) {
    (void)button; (void)user_data;
    GtkWidget *tree = TREE_ACTIVE_IN_ADMIN();
    if (!tree) { popup(GTK_MESSAGE_ERROR,"Aucun treeview trouvé dans admin."); return; }

    const char *needle = "";
    GtkWidget *e = W("entry34");
    if (e && GTK_IS_ENTRY(e)) needle = gtk_entry_get_text(GTK_ENTRY(e));

    load_tree_filtered(tree, needle ? needle : "");
    update_stats_from_file();
}

/* supprimer */
void on_khayri8_clicked(GtkButton *button, gpointer user_data) {
    (void)button; (void)user_data;

    GtkWidget *tree = TREE_ACTIVE_IN_ADMIN();
    if (!tree) { popup(GTK_MESSAGE_ERROR,"Aucun treeview trouvé dans admin."); return; }

    GtkTreeSelection *sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
    GtkTreeModel *model=NULL; GtkTreeIter iter;
    if (!gtk_tree_selection_get_selected(sel, &model, &iter)) {
        popup(GTK_MESSAGE_INFO,"Sélectionne un membre à supprimer.");
        return;
    }

    gint id=0;
    gtk_tree_model_get(model, &iter, COL_ID, &id, -1);

    if (!confirm_yesno("Confirmer la suppression ?")) return;

    char tmpname[256];
    snprintf(tmpname, sizeof(tmpname), "%s.tmp", FILE_MEMBRES);

    FILE *fin = fopen(FILE_MEMBRES, "r");
    if (!fin) { popup(GTK_MESSAGE_ERROR,"membre.txt introuvable"); return; }
    FILE *fout = fopen(tmpname, "w");
    if (!fout) { fclose(fin); popup(GTK_MESSAGE_ERROR,"Impossible d'écrire tmp"); return; }

    char line[4096];
    int found=0;
    while (fgets(line, sizeof line, fin)) {
        char **toks=NULL; int n=split_tokens(line, &toks);
        int cur = (n>=1) ? atoi(tok(toks,n,0)) : -1;
        if (toks) g_strfreev(toks);

        if (cur == id) { found=1; continue; }
        fputs(line, fout);
    }
    fclose(fin); fclose(fout);

    if (!found) { remove(tmpname); popup(GTK_MESSAGE_ERROR,"ID introuvable"); return; }
    remove(FILE_MEMBRES);
    rename(tmpname, FILE_MEMBRES);

    load_tree_all(tree);
    update_stats_from_file();
}

/* modifier admin -> ouvrir fiche préremplie */
void on_khayri7_clicked(GtkButton *button, gpointer user_data)
{
    (void)button; (void)user_data;

    GtkWidget *tree  = TREE_ACTIVE_IN_ADMIN();
    GtkWidget *admin = WIN_ADMIN();
    GtkWidget *fiche = WIN_FICHE();

    if (!tree)  { popup(GTK_MESSAGE_ERROR,"Aucun treeview trouvé dans admin."); return; }
    if (!admin) { popup(GTK_MESSAGE_ERROR,"Fenêtre admin introuvable."); return; }
    if (!fiche) { popup(GTK_MESSAGE_ERROR,"Fenêtre fiche introuvable (vérifie entry1)."); return; }

    GtkTreeSelection *sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
    GtkTreeModel *model=NULL; GtkTreeIter iter;
    if (!gtk_tree_selection_get_selected(sel, &model, &iter)) {
        popup(GTK_MESSAGE_INFO, "Sélectionne un membre dans le tableau.");
        return;
    }

    gint id=0;
    gtk_tree_model_get(model, &iter, COL_ID, &id, -1);

    FILE *f = fopen(FILE_MEMBRES, "r");
    if (!f) { popup(GTK_MESSAGE_ERROR,"membre.txt introuvable"); return; }

    char line[4096];
    int ok=0;
    char nom[256]="", prenom[256]="", date[64]="", sexe[16]="HOMME", email[256]="", tel[64]="", adr[256]="";

    while (fgets(line, sizeof line, f)) {
        char **toks=NULL; int n=split_tokens(line, &toks);
        if (n>=8 && atoi(tok(toks,n,0))==id) {
            g_strlcpy(nom,    tok(toks,n,1), sizeof(nom));
            g_strlcpy(prenom, tok(toks,n,2), sizeof(prenom));
            g_strlcpy(date,   tok(toks,n,3), sizeof(date));
            g_strlcpy(sexe,   tok(toks,n,4), sizeof(sexe));
            g_strlcpy(email,  tok(toks,n,5), sizeof(email));
            g_strlcpy(tel,    tok(toks,n,6), sizeof(tel));
            g_strlcpy(adr,    tok(toks,n,7), sizeof(adr));
            ok=1;
        }
        if (toks) g_strfreev(toks);
        if (ok) break;
    }
    fclose(f);

    if (!ok) { popup(GTK_MESSAGE_ERROR,"Membre introuvable dans membre.txt"); return; }

    g_edit_id = id;

    entry_set("entry1", nom);
    entry_set("entry2", prenom);
    entry_set("entry4", email);
    entry_set("entry7", tel);
    entry_set("entry8", adr);

    int jj=1, mm=1, aa=2000;
    sscanf(date, "%d/%d/%d", &jj, &mm, &aa);
    spin_set("spinbutton3", jj);
    spin_set("spinbutton2", mm);
    spin_set("spinbutton1", aa);

    GtkWidget *rH = W("khayri180");
    GtkWidget *rF = W("KHAYRI180");
    if (rH && rF) {
        if (g_ascii_strcasecmp(sexe,"FEMME")==0)
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rF), TRUE);
        else
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rH), TRUE);
    }

    gtk_widget_hide(admin);
    gtk_widget_show_all(fiche);
    gtk_window_present(GTK_WINDOW(fiche));
}

/* modifier fiche -> update fichier -> refresh treeview -> retour admin */
void on_khayri41_clicked(GtkButton *button, gpointer user_data)
{
    (void)button; (void)user_data;

    if (g_edit_id <= 0) {
        popup(GTK_MESSAGE_INFO,"Clique d'abord Modifier (admin) après sélection.");
        return;
    }

    const char *nom    = entry_get("entry1");
    const char *prenom = entry_get("entry2");
    const char *email  = entry_get("entry4");
    const char *tel    = entry_get("entry7");
    const char *adr    = entry_get("entry8");

    /* ✅ contrôle téléphone */
    if (!validate_tel_or_popup(tel)) return;

    int jj = spin_get("spinbutton3", 1);
    int mm = spin_get("spinbutton2", 1);
    int aa = spin_get("spinbutton1", 2000);

    char date[64];
    snprintf(date, sizeof(date), "%02d/%02d/%04d", jj, mm, aa);

    char sexe[16] = "HOMME";
    GtkWidget *rF = W("KHAYRI180");
    if (rF && GTK_IS_TOGGLE_BUTTON(rF) && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rF)))
        strcpy(sexe, "FEMME");

    if (!update_member_by_id(g_edit_id, nom, prenom, date, sexe, email, tel, adr)) {
        popup(GTK_MESSAGE_ERROR,"Erreur modification (membre.txt).");
        return;
    }

    GtkWidget *tree = TREE_ACTIVE_IN_ADMIN();
    if (tree) load_tree_all(tree);
    update_stats_from_file();

    g_edit_id = -1;

    GtkWidget *admin = WIN_ADMIN();
    GtkWidget *fiche = WIN_FICHE();
    if (fiche) gtk_widget_hide(fiche);
    if (admin) { gtk_widget_show_all(admin); gtk_window_present(GTK_WINDOW(admin)); }
}

/* ajouter */
void on_khayri40_clicked(GtkButton *button, gpointer user_data)
{
    (void)button; (void)user_data;

    int id = get_next_id();

    const char *nom    = entry_get("entry1");
    const char *prenom = entry_get("entry2");
    const char *email  = entry_get("entry4");
    const char *tel    = entry_get("entry7");
    const char *adr    = entry_get("entry8");

    /* ✅ contrôle téléphone */
    if (!validate_tel_or_popup(tel)) return;

    int jj = spin_get("spinbutton3", 1);
    int mm = spin_get("spinbutton2", 1);
    int aa = spin_get("spinbutton1", 2000);

    char date[64];
    snprintf(date, sizeof(date), "%02d/%02d/%04d", jj, mm, aa);

    char sexe[16] = "HOMME";
    GtkWidget *rF = W("KHAYRI180");
    if (rF && GTK_IS_TOGGLE_BUTTON(rF) && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rF)))
        strcpy(sexe, "FEMME");

    FILE *f = fopen(FILE_MEMBRES, "a");
    if (!f) { popup(GTK_MESSAGE_ERROR,"Impossible d'écrire membre.txt"); return; }

    /* format minimal 8 champs */
    fprintf(f, "%d;%s;%s;%s;%s;%s;%s;%s\n", id, nom, prenom, date, sexe, email, tel, adr);
    fclose(f);

    GtkWidget *tree = TREE_ACTIVE_IN_ADMIN();
    if (tree) load_tree_all(tree);

    update_stats_from_file();
    popup(GTK_MESSAGE_INFO,"Membre ajouté ✅");
}

/* statistique */
void on_khayri900_clicked(GtkButton *button, gpointer user_data)
{
    (void)button; (void)user_data;
    update_stats_from_file();
}

/* wrappers */
void on_khayri81_clicked(GtkButton *button, gpointer user_data) { on_khayri41_clicked(button, user_data); }
void on_khayri80_clicked(GtkButton *button, gpointer user_data) { on_khayri41_clicked(button, user_data); }
void on_fiche_modifier_clicked(GtkButton *button, gpointer user_data) { on_khayri41_clicked(button, user_data); }

/* divers */
void on_treeview1_row_activated(GtkTreeView *treeview, GtkTreePath *path,
                                GtkTreeViewColumn *column, gpointer user_data)
{ (void)treeview; (void)path; (void)column; (void)user_data; }

void on_window_destroy(GtkWidget *widget, gpointer user_data)
{ (void)widget; (void)user_data; gtk_main_quit(); }

