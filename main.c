# include <stdio.h>
# include <string.h>
# include <ctype.h>
# include <time.h>
# include <gtk-3.0/gtk/gtk.h>

/*
gcc main.c -o main $(pkg-config --cflags --libs gtk+-3.0)
*/

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
	gpointer win;
}STR_OPS;

void end_program(GtkWidget *wid, gpointer ptr, STR_OPS *user_data);
void correctIncrease(GtkWidget *wid, gpointer ptr);
void incorrectIncrease(GtkWidget *wid, gpointer ptr);
void browseFile(GtkWidget *wid, GdkEventKey *event, STR_OPS *user_data);
void read_words (STR_OPS *user_data);
void calWPM(long int timeElapsed);
void calWpmAndReset(GtkWidget *wid, GdkEventKey *event, STR_OPS *user_data);
void reset(STR_OPS *user_data, char fileName[]);
void resetCallback(GtkWidget *wid, GdkEventKey *event, STR_OPS *user_data);
static gboolean keyCallback(GtkWidget *wid, GdkEventKey *event, gpointer user_data);
static void load_css();


int main(int argc, char *argv[]){
	struct STR_OPS data;
	FILE *fp;
	fp = fopen("./exampleText.txt", "rb");

	gtk_init(&argc, &argv);

	GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	load_css();

	GtkWidget *tbl = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	GtkWidget *tbl_1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	GtkWidget *tbl_2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	GtkWidget *tbl_3 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	GtkWidget *tbl_4 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

	GtkWidget *text = gtk_label_new(data.str);
	GtkWidget *typed = gtk_label_new(" ");
	GtkWidget *btn = gtk_button_new_with_label("Yametekudastop");
	GtkWidget *correct = gtk_label_new("Correct: 0");
	GtkWidget *wrong = gtk_label_new("Incorrect: 0");
	GtkWidget *fileChooser = gtk_button_new_with_label("Browse");
	GtkWidget *resetBtn = gtk_button_new_with_label("Reset");

	data.text = text;
	data.label = typed;
	data.correctLabel = correct;
	data.incorrectLabel = wrong;
	data.curStrIndex = 0;
	data.filePath = fp;
	data.startTime = time(0);
	data.win = win;

	read_words(&data);
	data.strLen = strlen(data.str);

	gtk_window_set_title(GTK_WINDOW (win), "Typing Game");
	gtk_window_set_default_size(GTK_WINDOW (win), 1200, 100);
	gtk_container_set_border_width(GTK_CONTAINER (tbl), 10);

	g_signal_connect(btn, "button_release_event", G_CALLBACK(calWpmAndReset), &data);
	g_signal_connect(fileChooser, "button_release_event", G_CALLBACK(browseFile), &data);
	g_signal_connect(resetBtn, "button_release_event", G_CALLBACK(resetCallback), &data);
	g_signal_connect(win, "delete_event", G_CALLBACK(end_program), &data);
	g_signal_connect(win, "key-press-event", G_CALLBACK(keyCallback), &data);

	gtk_widget_set_name(text, "exampleText");
	gtk_widget_set_name(typed, "typedText");
	gtk_widget_set_name(tbl_2, "typeBox");

	gtk_box_pack_start(GTK_BOX (tbl), tbl_1, TRUE, TRUE, 10);
	gtk_box_pack_start(GTK_BOX (tbl), tbl_2, TRUE, TRUE, 10);
	gtk_box_pack_start(GTK_BOX (tbl), tbl_3, TRUE, TRUE, 10);
	gtk_box_pack_start(GTK_BOX (tbl), tbl_4, TRUE, TRUE, 10);

	gtk_box_pack_start(GTK_BOX (tbl_1), text, TRUE, TRUE, 10);
	gtk_box_pack_start(GTK_BOX (tbl_2), typed, TRUE, TRUE, 10);
	gtk_box_pack_end(GTK_BOX (tbl_3), correct, TRUE, TRUE, 10);
	gtk_box_pack_end(GTK_BOX (tbl_3), wrong, TRUE, TRUE, 10);
	gtk_box_pack_start(GTK_BOX (tbl_4), fileChooser, TRUE, TRUE, 10);
	gtk_box_pack_start(GTK_BOX (tbl_4), btn, TRUE, TRUE, 10);
	gtk_box_pack_start(GTK_BOX (tbl_4), resetBtn, TRUE, TRUE, 10);

	gtk_container_add(GTK_CONTAINER (win), tbl);

	gtk_widget_show_all(win);
	gtk_main();
	return 0;
}


void end_program(GtkWidget *wid, gpointer ptr, STR_OPS *user_data){
	fclose(user_data->filePath);
	gtk_main_quit();
}

void correctIncrease(GtkWidget *wid, gpointer ptr){
	char buffer[30];
	correct++;
	sprintf(buffer, "Correct: %d", correct);
	gtk_label_set_text(GTK_LABEL (ptr), buffer);
}

void browseFile(GtkWidget *wid, GdkEventKey *event, STR_OPS *user_data){
	GtkWidget *fileDialog = gtk_file_chooser_dialog_new("Open File",
					                                      GTK_WINDOW(user_data->win),
					                                      GTK_FILE_CHOOSER_ACTION_OPEN,
					                                      "_Cancel",
					                                      GTK_RESPONSE_CANCEL,
					                                      "_Open",
					                                      GTK_RESPONSE_ACCEPT,
					                                      NULL);
	gint res = gtk_dialog_run (GTK_DIALOG (fileDialog));
	if (res == GTK_RESPONSE_ACCEPT){
			char *filename;
			GtkFileChooser *chooser = GTK_FILE_CHOOSER (fileDialog);
			filename = gtk_file_chooser_get_filename (chooser);
			reset(user_data, filename);
			g_free(filename);
		}
	gtk_widget_destroy(fileDialog);
}

void incorrectIncrease(GtkWidget *wid, gpointer ptr){
	char buffer[30];
	incorrect++;
	sprintf(buffer, "Incorrect: %d", incorrect);
	gtk_label_set_text(GTK_LABEL (ptr), buffer);
}

void read_words (STR_OPS *user_data) {
    char buffer[128];
    char tBuffer[128];
    int i = 127;
    int bufferLen;

    if (fgets(buffer, 127, user_data->filePath) != NULL) {
        bufferLen = strlen(buffer);

		if(bufferLen == 0 ||(bufferLen == 1 && (buffer[0] <= 31 || buffer[0] >= 127))){
			read_words(user_data);
		}else if(isspace(buffer[bufferLen-1])){
    		buffer[bufferLen-1] = '\0';
    		for(int k=bufferLen; k>-1; k--){
    			if(isspace(buffer[k]))
    				buffer[k] = '\0';
    			else
    				break;
    		}
    		user_data->strLen = strlen(buffer);

		    strcpy(user_data->str, buffer);
		    user_data->str[user_data->strLen]='\0';
			/*
			For replacing space
			for(int i=0; i<=bufferLen; i++){
				if(isspace(buffer[i])){
					buffer[i] = '>';
				}
			}
			*/
		    gtk_label_set_text(GTK_LABEL (user_data->text), buffer);
    	}else{
    		user_data->strLen = strlen(buffer);

		    strcpy(user_data->str, buffer);
		    user_data->str[user_data->strLen]='\0';
			/*
			For replacing space
			for(int i=0; i<=bufferLen; i++){
				if(isspace(buffer[i])){
					buffer[i] = '>';
				}
			}
			*/
		    gtk_label_set_text(GTK_LABEL (user_data->text), buffer);
    	}
    }else{
    	time_t endTime = time(0);
    	g_printerr("EOF\n");
    	calWPM(endTime-user_data->startTime);
    	fclose(user_data->filePath);
    	exit(0);
    }
}

void calWpmAndReset(GtkWidget *wid, GdkEventKey *event, STR_OPS *user_data){
	time_t endTime = time(0);
	calWPM(endTime - user_data->startTime);
	reset(user_data, NULL);
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

		if(d->str[d->curStrIndex] <= 31 || d->str[d->curStrIndex] >= 127){
			if(d->curStrIndex + 1 < d->curStrIndex -1)
				d->curStrIndex++;
			else{
				d->curStrIndex = 0;
				d->typedStr[d->curStrIndex] = '\0';
				gtk_label_set_text(GTK_LABEL (d->label), d->typedStr);
				read_words(user_data);
			}
		}
		else if(d->str[d->curStrIndex] == key){
			correctIncrease(wid, d->correctLabel);
			d->typedStr[d->curStrIndex] = key;
			d->typedStr[d->curStrIndex + 1] = '\0';
			gtk_label_set_text(GTK_LABEL (d->label), strcat(d->typedStr, "█"));

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

static void load_css(){
	GtkCssProvider *cssProvider;
	GdkDisplay *display;
	GdkScreen *screen;

	const gchar *cssFile = "styles.css";
	GFile *f = g_file_new_for_path(cssFile);
	GError *error = 0;

	cssProvider = gtk_css_provider_new();
	display = gdk_display_get_default();
	screen = gdk_display_get_default_screen(display);

	gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	gtk_css_provider_load_from_file(cssProvider, f, &error);

	g_object_unref(cssProvider);
}

void reset(STR_OPS *user_data, char fileName[]){
	if(fileName != NULL){
		fclose(user_data->filePath);
		FILE *fp;
		fp = fopen(fileName, "rb");
		user_data->filePath = fp;
	}else{
		fseek(user_data->filePath, 0, SEEK_SET);
	}
	correct = 0;
	incorrect = 0;
	word = 0;

	gtk_label_set_text(GTK_LABEL (user_data->correctLabel), "Correct: 0");
	gtk_label_set_text(GTK_LABEL (user_data->incorrectLabel), "Incorrect: 0");
	user_data->curStrIndex = 0;
	user_data->startTime  = time(0);
	strcpy(user_data->typedStr, "");

	gtk_label_set_text(GTK_LABEL (user_data->label), user_data->typedStr);

	read_words(user_data);
}

void resetCallback(GtkWidget *wid, GdkEventKey *event, STR_OPS *user_data){
	reset(user_data, NULL);
}
