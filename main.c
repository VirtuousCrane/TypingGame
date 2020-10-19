# include <stdio.h>
# include <string.h>
# include <ctype.h>
# include <gtk-2.0/gtk/gtk.h>

int correct = 0;
int incorrect = 0;

typedef struct STR_OPS{
	int strLen;
	int curStrIndex;
	char str[128];
	char typedStr[128];
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
static gboolean keyCallback(GtkWidget *wid, GdkEventKey *event, gpointer user_data);


int main(int argc, char *argv[]){
	FILE *fp;
	fp = fopen("./exampleText.txt", "rb");

	struct STR_OPS data;
	data.curStrIndex = 0;
	data.filePath = fp;
	
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
	g_signal_connect(win, "key-press-event", G_CALLBACK(keyCallback), &data);
	
	gtk_table_attach_defaults(GTK_TABLE (tbl), text, 0, 2, 0, 1); // Row 0 Space 0-1
	gtk_table_attach_defaults(GTK_TABLE (tbl), typed, 0, 2, 2, 3); // Row 2 Space 0-1
	gtk_table_attach_defaults(GTK_TABLE (tbl), correct, 2, 3, 0, 1); // Row 0 Space 2
	gtk_table_attach_defaults(GTK_TABLE (tbl), wrong, 2, 3, 1, 2); // Row 1 Space 2

	
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
	static char leftover[1024];
    char buffer[1024];
    int i = 127;
    int leftoverIndex = 0;
    int bufferLen, leftoverLen = strlen(leftover);
    g_printerr("%d", leftoverLen);

    if (leftoverLen > 0){
    	strcpy(buffer, leftover);
    	bufferLen = strlen(buffer);
    	leftover[0] = '\0';

    	if(bufferLen > 127){
    		int i = 127;
    		for(i, leftoverIndex; i <= bufferLen; i++, leftoverIndex++){
    			if(i == bufferLen)
    				leftover[leftoverIndex] = '\0';
    			else
    				leftover[leftoverIndex] = buffer[i];
    		}
    		buffer[127] = '\0';
    	}else{
    		user_data->strLen = strlen(buffer);
    	}

        strcpy(user_data->str, buffer);
        user_data->str[strlen(buffer)] = '\0';
        gtk_label_set_text(GTK_LABEL (user_data->text), buffer);

    }else if (fscanf(user_data->filePath, "%[^\n]\n", buffer) == 1) {
        bufferLen = strlen(buffer);
        g_printerr("%d", bufferLen);

    	if(isspace(buffer[bufferLen-1])){
    		buffer[bufferLen-1] = '\0';
    		user_data->strLen = bufferLen;
    	}

    	if(bufferLen > 127){
    		g_printerr("Overflown\n");
    		for(i, leftoverIndex; i <= bufferLen; i++, leftoverIndex++){
    			g_printerr("%d", i);
    			if(i == bufferLen)
    				leftover[leftoverIndex] = '\0';
    			else
    				leftover[leftoverIndex] = buffer[i];
    		}
    		g_printerr("%s\n", leftover);
    		buffer[127] = '\0';
    		user_data->strLen = bufferLen - (leftoverIndex-1);
    	}else{
    		user_data->strLen = strlen(buffer);
    	}

        strcpy(user_data->str, buffer);
        user_data->str[user_data->strLen]='\0';
        gtk_label_set_text(GTK_LABEL (user_data->text), buffer);
    }else{
    	g_printerr("EOF\n");
    	fclose(user_data->filePath);
    	exit(0);
    }
}

static gboolean keyCallback(GtkWidget *wid, GdkEventKey *event, gpointer user_data){
	struct STR_OPS *d = user_data;
	char key = event->keyval;
	char temp[2];

	sprintf(temp, "%c", key);
	if(key > 31 && key < 127){
		g_printerr("%c %d\n", key, key);
		// gtk_label_set_text(GTK_LABEL (d->label), temp);
		
		if(d->str[d->curStrIndex] == key){
			g_printerr("Correct\n");
			correctIncrease(wid, d->correctLabel);
			d->typedStr[d->curStrIndex] = key;
			d->typedStr[d->curStrIndex + 1] = '\0';
			gtk_label_set_text(GTK_LABEL (d->label), d->typedStr);

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