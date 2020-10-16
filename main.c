# include <stdio.h>
# include <string.h>
# include <gtk-2.0/gtk/gtk.h>

typedef struct {
	int strLen;
	int curStrIndex;
	char str[10];
} STR_OPS;

void end_program(GtkWidget *wid, gpointer ptr){
	gtk_main_quit();
}

static gboolean key_event(GtkWidget *wid, 
							GdkEventKey *event, 
							gpointer label,
							STR_OPS d){
	char key = event->keyval;
	char temp[2];
	sprintf(temp, "%c", key);
	if(key > 31 && key < 127){
		g_printerr("%c %d\n", key, key);
		gtk_label_set_text(GTK_LABEL (label), temp);
		
		if(d.str[d.curStrIndex] == key){
			g_printerr("Correct\n");
			if(d.curStrIndex < d.strLen-1)
				d.curStrIndex++;
		}
	}
	return FALSE;
}

int main(int argc, char *argv[]){
	STR_OPS data;
	data.curStrIndex = 0;
	strcpy(data.str, "Test Text");
	data.strLen = strlen(data.str);
	g_printerr("%d\n", data.strLen);
	// FILE *f = fopen("./exampleText.txt");	
	
	gtk_init(&argc, &argv);
	
	GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *tbl = gtk_table_new(2, 2, TRUE);
	GtkWidget *text = gtk_label_new(data.str);
	GtkWidget *typed = gtk_label_new(" ");
	
	g_signal_connect(win, "delete_event", G_CALLBACK(end_program), NULL);
	g_signal_connect(win, "key-press-event", G_CALLBACK(key_event), typed, data); //  TODO: Fix Error
	
	gtk_table_attach_defaults(GTK_TABLE (tbl), text, 0, 2, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE (tbl), typed, 0, 2, 1, 2);
	
	gtk_container_add(GTK_CONTAINER (win), tbl);
	
	gtk_widget_show_all(win);
	gtk_main();
	return 0;
}
