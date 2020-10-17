# include <stdio.h>
# include <string.h>
# include <gtk-2.0/gtk/gtk.h>

int correct = 0;
int incorrect = 0;

typedef struct STR_OPS{
	int strLen;
	int curStrIndex;
	char str[256];
	FILE *filePath;
	gpointer label;
	gpointer text;
	gpointer correctLabel;
	gpointer incorrectLabel;
}STR_OPS;

void end_program(GtkWidget *wid, gpointer ptr){
	gtk_main_quit();
}

void correctIncrease(GtkWidget *wid, gpointer ptr){
	char buffer[30];
	correct++;
	sprintf(buffer, "Correct: %d", correct);
	gtk_label_set_text(GTK_LABEL (ptr), buffer);
}

void incorrectIncrease(GtkWidget *wid, gpointer ptr){
	char buffer[30];
	incorrect++;
	sprintf(buffer, "Incorrect: %d", incorrect);
	gtk_label_set_text(GTK_LABEL (ptr), buffer);
}

void read_words (STR_OPS *user_data) {
    char buffer[256];
    int bufferLen;
    if (fscanf(user_data->filePath, "%256[^\n]\n", buffer) == 1) {	// TODO: 256 char limit doesn't work.
        puts(buffer);
        bufferLen = strlen(buffer);
        strcpy(user_data->str, buffer);
        user_data->str[bufferLen]='\0';
        user_data->strLen = bufferLen;
        gtk_label_set_text(GTK_LABEL (user_data->text), buffer);
    }else{
    	g_printerr("Cannot read\n");
    	exit(0);
    }
}

static gboolean key_event(GtkWidget *wid, 
							GdkEventKey *event, 
							gpointer user_data){
	struct STR_OPS *d = user_data;
	char key = event->keyval;
	char temp[2];

	sprintf(temp, "%c", key);
	if(key > 31 && key < 127){
		g_printerr("%c %d\n", key, key);
		gtk_label_set_text(GTK_LABEL (d->label), temp);
		
		if(d->str[d->curStrIndex] == key){
			g_printerr("Correct\n");
			correctIncrease(wid, d->correctLabel);
			if(d->curStrIndex < d->strLen-1){
				d->curStrIndex = d->curStrIndex+1;
			}
			else{
				d->curStrIndex = 0;
				read_words(user_data);
			}
		}else{
			incorrectIncrease(wid, d->incorrectLabel);
		}
	}
	return FALSE;
}

int main(int argc, char *argv[]){
	FILE *fp;
	fp = fopen("./exampleText.txt", "rb");

	struct STR_OPS data;
	data.curStrIndex = 0;
	data.filePath = fp;
	// strcpy(data.str, "Test Text");
	
	gtk_init(&argc, &argv);
	
	GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *tbl = gtk_table_new(3, 3, TRUE);
	GtkWidget *text = gtk_label_new(data.str);
	GtkWidget *typed = gtk_label_new(" ");
	GtkWidget *correct = gtk_label_new("Correct: 0");
	GtkWidget *wrong = gtk_label_new("Incorrect: 0");
	
	data.text = text;
	data.label = typed;
	data.correctLabel = correct;
	data.incorrectLabel = wrong;

	read_words(&data);
	data.strLen = strlen(data.str);
	
	g_signal_connect(win, "delete_event", G_CALLBACK(end_program), NULL);
	g_signal_connect(win, "key-press-event", G_CALLBACK(key_event), &data);
	
	gtk_table_attach_defaults(GTK_TABLE (tbl), text, 0, 2, 0, 1); // Row 0 Space 0-1
	gtk_table_attach_defaults(GTK_TABLE (tbl), typed, 0, 2, 2, 3); // Row 2 Space 0-1
	gtk_table_attach_defaults(GTK_TABLE (tbl), correct, 2, 3, 0, 1); // Row 0 Space 2
	gtk_table_attach_defaults(GTK_TABLE (tbl), wrong, 2, 3, 1, 2); // Row 1 Space 2

	
	gtk_container_add(GTK_CONTAINER (win), tbl);
	
	gtk_widget_show_all(win);
	gtk_main();
	return 0;
}
