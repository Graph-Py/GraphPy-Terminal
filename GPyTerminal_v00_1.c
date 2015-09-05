// The application is named as GraphPy //

/*
GPyTerminal Version 0.0.1
file name : GPyTermina_v00_1.c
This code is the updation to code: termin001.c
*/

/*
Compile this code with
`pkg-config --cflags gtk+-3.0 --cflags gdk-3.0`
`pkg-config --libs gtk+-3.0 --libs gdk-3.0`
*/

#include <gtk/gtk.h>

#define UI_DESIGN_FILE "GPyTerminalUI001.glade"
// Store the widgets which may need to be accessed //
typedef struct
{
	GtkWidget		*window;
	GtkWidget		*text_view;
	GtkWidget		*statusbar;
}TerminalView;

// Window callbacks //
void on_window_destroy (GObject *object, TerminalView *terminal);
gboolean on_textview_key_press_event (GtkWidget *widget, GdkEventKey *event, TerminalView *terminal);

// Misc. Functions //
gboolean init_app (TerminalView *terminal);
void init_terminal (TerminalView *terminal);

// Main function //

int
main(int argc, char *argv[])
{
	TerminalView		*terminal;
	
	// Allocate memmory //
	terminal = g_slice_new (TerminalView);
	
	// Initialize gtk //
	gtk_init (&argc, &argv);
	
	// Initialize our application //
	if (init_app (terminal)) return 1; // here the init_app return value changed from the one used in previous version termin001.c
	
	// Initialize the terminal window //
	init_terminal (terminal);
	
	// show all widgets //
	gtk_widget_show (terminal->window);
	
	// Gtk main function //
	gtk_main ();
	
	// Free up the memmory allocated to the TerminalView struct //
	g_slice_free (TerminalView, terminal); 
	return 0;
}

//******************************************************************************//
// Function: INIT_APP								//
// This function builds and initializes widgets in the glade ui design		//
//******************************************************************************//
gboolean
init_app (TerminalView *terminal)
{
	GtkBuilder	*builder;
	GError		*err = NULL;
	
	// create the builder object and initialize glade ui file //
	builder = gtk_builder_new ();
	if (gtk_builder_add_from_file (builder, UI_DESIGN_FILE, &err) == 0)
	{
		g_warning ("Error Reading Glade/UI file:\n%s", err->message);
		return TRUE;
	}
	
	// Get the widgets //
	terminal->window	= GTK_WIDGET (gtk_builder_get_object (builder, "window"));
	terminal->text_view	= GTK_WIDGET (gtk_builder_get_object (builder, "textview"));
	terminal->statusbar	= GTK_WIDGET (gtk_builder_get_object (builder, "statusbar"));
	
	// connect the signals //
	gtk_builder_connect_signals (builder, terminal);
	
	
	g_object_unref (G_OBJECT (builder));
	
	return FALSE;
}

//******************************************************************************//
// Function: INIT_TERMINAL							//
// This function initializes the terminal window with necessary tags and marks	//
//******************************************************************************//

void
init_terminal (TerminalView *terminal)
{
	GtkTextBuffer		*buffer;
	GtkTextIter		start, end;
	
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (terminal->text_view));
	gtk_text_buffer_create_tag (buffer, "prompt", "foreground", "blue", NULL);
	gtk_text_buffer_create_tag (buffer, "normal", "foreground", "black", NULL);
	gtk_text_buffer_create_tag (buffer, "error", "foreground", "red", NULL);
	
	// new tags has been introduced in this version //
	gtk_text_buffer_create_tag (buffer, "can-edit", "editable", TRUE, NULL);
	gtk_text_buffer_create_tag (buffer, "not-editable", "editable", FALSE, NULL);
	
	// Insert a prompt on the textview buffer and make it not editable for the user //
	gtk_text_buffer_get_end_iter (buffer, &end);
	gtk_text_buffer_create_mark  (buffer, "input-line", &end, TRUE);
	gtk_text_buffer_insert (buffer, &end, ">>> ", -1);
	gtk_text_buffer_get_end_iter (buffer, &end);
	gtk_text_buffer_get_iter_at_offset (buffer, &start, 0);
	gtk_text_buffer_apply_tag_by_name (buffer,"prompt", &start, &end);
	gtk_text_buffer_apply_tag_by_name (buffer,"not-editable", &start, &end);
	gtk_text_buffer_create_mark (buffer, "input", &end, TRUE);
}


//******************************************************************************//
// Function: KEY_PRESS_EVENT							//
// Function to handle Key press events on text buffer				//
//******************************************************************************//

gboolean
on_textview_key_press_event (GtkWidget *widget, GdkEventKey *event, TerminalView *terminal)
{
	GdkModifierType	modifier_mask;
	guint			event_state;
	const gchar		*prompt_mark = ">>> ";
	
	modifier_mask = gtk_accelerator_get_default_mod_mask ();
	event_state = event->state & modifier_mask;
	
	if ((event->keyval == GDK_KEY_D) && (event->state == GDK_CONTROL_MASK))
	{
		// This should terminate all executions	//
		g_message ("CONTROL+D is pressed\n");
	}
	else if ((event->keyval == GDK_KEY_Return) || (event->keyval == GDK_KEY_KP_Enter))
	{
		GtkTextBuffer	*buffer;
		GtkTextMark	*lin_mark, *inp_mark;
		GtkTextIter	inp, start, end, lin;
		gchar		*line;
		gchar		*current_command;
		
		
// Possible Bug Notification	//
		// Possible bug: make sure that the widget here is the TextView...			//
		// there will be a chance to get errors when multiple widgets present in the ui.	//						//
		// Get the marks //
		buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));
		
		inp_mark = gtk_text_buffer_get_mark (buffer, "input");
		lin_mark = gtk_text_buffer_get_mark (buffer, "input-line");
		
		// Get The Command-Line //
		gtk_text_buffer_get_iter_at_mark (buffer, &inp, inp_mark);
		gtk_text_buffer_get_end_iter (buffer, &end);
		current_command = gtk_text_buffer_get_text (buffer, &inp, &end, FALSE);
		
		// add "current_command" to history //
		// history_add (current_command);	// The function is to be written

// Purpose: Test only, should be removed after test //
	
		gchar	*message;
		message = g_strdup_printf ("\n Executing: %s ", current_command);
		gtk_text_buffer_get_end_iter (buffer, &end);
		gtk_text_buffer_insert (buffer, &end, message, -1);
		g_message ("You have entered: %s", current_command);
//

		// After getting the "current_command" it should run in python //
		 

		// Prepare a new line //
		gtk_text_buffer_get_end_iter (buffer, &end);
		gtk_text_buffer_insert (buffer, &end, "\n", -1);
		gtk_text_buffer_get_end_iter (buffer, &end);
		gtk_text_buffer_move_mark (buffer, lin_mark, &end);

		gtk_text_buffer_insert_at_cursor (buffer, prompt_mark, -1);
		gtk_text_buffer_get_end_iter (buffer, &end);
		gtk_text_buffer_move_mark (buffer, inp_mark, &end);
		gtk_text_buffer_place_cursor (buffer, &end);

		// apply tag to the new line //
		gtk_text_buffer_get_iter_at_mark (buffer, &lin, lin_mark);
		gtk_text_buffer_apply_tag_by_name (buffer, "prompt", &lin, &end);
		
		// make the previous lines not editable //
		gtk_text_buffer_get_iter_at_offset (buffer, &start, 0);
		gtk_text_buffer_get_end_iter (buffer, &end);
		gtk_text_buffer_apply_tag_by_name (buffer,"not-editable", &start, &end);
		
		// Not sure whether it is the right method to prevent the event from propagating further //
		return TRUE;
	}
	return FALSE;
}

//******************************************************************************//
// Function: ON_WINDOW_DESTROY									 	//
// Function to handle window destroy event								//
//******************************************************************************//

void
on_window_destroy (GObject *object, TerminalView *terminal)
{
	gtk_main_quit ();
}
