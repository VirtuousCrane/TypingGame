# include <stdio.h>
# include <string.h>
# include <ctype.h>
# include <time.h>
# include <gtk-2.0/gtk/gtk.h>

int correct = 0;
int incorrect = 0;
int word = 0;

typedef struct STR_OPS{
	int strLen;
	int curStrIndex;
	char str[128];
	char typedStr[128];
	time_t startTime;
	FILE *filePath;
	gpointer label;
	gpointer text;
	gpointer correctLabel;
	gpointer incorrectLabel;
}STR_OPS;


void end_program(GtkWidget *wid, gpointer ptr);
void correctIncrease(GtkWidget *wid, gpointer ptr);
void incorrectIncrease(GtkWidget *wid, gpointer ptr);
void read_words (STR_OPS *user_data);
void calWPM(long int timeElapsed);
static gboolean keyCallback(GtkWidget *wid, GdkEventKey *event, gpointer user_data);


int main(int argc, char *argv[]){
	FILE *fp;
	fp = fopen("./exampleText.txt", "rb");

	struct STR_OPS data;
	data.curStrIndex = 0;
	data.filePath = fp;
	data.startTime = time(0);
	
	gtk_init(&argc, &argv);
	
	GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *tbl = gtk_table_new(3, 5, TRUE);
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
	
	gtk_container_set_border_width(GTK_CONTAINER (tbl), 10);
	gtk_table_set_col_spacing(GTK_TABLE (tbl), 2, 5);
	
	g_signal_connect(win, "delete_event", G_CALLBACK(end_program), NULL);
	g_signal_connect(win, "key-press-event", G_CALLBACK(keyCallback), &data);
	
	gtk_table_attach_defaults(GTK_TABLE (tbl), text, 0, 4, 0, 1); // Row 0 Space 0-1
	gtk_table_attach_defaults(GTK_TABLE (tbl), typed, 0, 4, 2, 3); // Row 2 Space 0-1
	gtk_table_attach_defaults(GTK_TABLE (tbl), correct, 4, 5, 0, 1); // Row 0 Space 2
	gtk_table_attach_defaults(GTK_TABLE (tbl), wrong, 4, 5, 1, 2); // Row 1 Space 2

	
	gtk_container_add(GTK_CONTAINER (win), tbl);
	
	gtk_widget_show_all(win);
	gtk_main();
	return 0;
}


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
    char buffer[128];
    int i = 127;
    int bufferLen;
    g_printerr("===\n%d words\n===\n", word);

    if (fgets(buffer, 127, user_data->filePath) != NULL) {
        bufferLen = strlen(buffer);
        g_printerr("Buffer Len: %d\n", bufferLen);
		
		g_printerr("Last Char: %c\n", buffer[bufferLen-1]);
    	if(isspace(buffer[bufferLen-1])){
    		g_printerr("IsSpace\n");
    		buffer[bufferLen-1] = '\0';
    		user_data->strLen = strlen(buffer);
    	}else{
    		user_data->strLen = strlen(buffer);
    	}

        strcpy(user_data->str, buffer);
        user_data->str[user_data->strLen]='\0';
        gtk_label_set_text(GTK_LABEL (user_data->text), buffer);
    }else{
    	time_t endTime = time(0);
    	g_printerr("Elapsed: %ld", endTime-user_data->startTime);
    	g_printerr("EOF\n");
    	calWPM(endTime-user_data->startTime);
    	fclose(user_data->filePath);
    	exit(0);
    }
}

void calWPM(long int timeElapsed){
	char msg[128];
	sprintf(msg, "Your WPM is %.2f.\nYour accuracy is %.2f percent.", (word+1)/(timeElapsed/60.0), ((correct-incorrect)/(float) correct)*100);
	GtkWidget *dialog = gtk_message_dialog_new(NULL, 
												GTK_DIALOG_MODAL,
												GTK_MESSAGE_INFO,
												GTK_BUTTONS_OK,
												msg);
	gtk_dialog_run(GTK_DIALOG (dialog));
	gtk_widget_destroy(dialog);
}

static gboolean keyCallback(GtkWidget *wid, GdkEventKey *event, gpointer user_data){
	struct STR_OPS *d = user_data;
	char key = event->keyval;
	char temp[2];

	sprintf(temp, "%c", key);
	if(key > 31 && key < 127){
		g_printerr("%c %d\n", key, key);
		
		if(d->str[d->curStrIndex] == key){
			g_printerr("Correct\n");
			correctIncrease(wid, d->correctLabel);
			d->typedStr[d->curStrIndex] = key;
			d->typedStr[d->curStrIndex + 1] = '\0';
			gtk_label_set_text(GTK_LABEL (d->label), d->typedStr);
			
			if (key == ' ')
				word++;

			if(d->curStrIndex < d->strLen - 1)
				d->curStrIndex = d->curStrIndex + 1;
			else{
				d->curStrIndex = 0;
				d->typedStr[d->curStrIndex] = '\0';
				gtk_label_set_text(GTK_LABEL (d->label), d->typedStr);
				read_words(user_data);
			}
		}else{
			incorrectIncrease(wid, d->incorrectLabel);
		}
	}
	return FALSE;
}
