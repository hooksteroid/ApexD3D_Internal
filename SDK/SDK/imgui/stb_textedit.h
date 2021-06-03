////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////
////   Header-file mode
////
////

#ifndef INCLUDE_STB_TEXTEDIT_H
#define INCLUDE_STB_TEXTEDIT_H

////////////////////////////////////////////////////////////////////////
//
//     STB_TexteditState
//
// Definition of STB_TexteditState which you should store
// per-textfield; it includes cursor position, selection state,
// and undo state.
//

#ifndef STB_TEXTEDIT_UNDOSTATECOUNT
#define STB_TEXTEDIT_UNDOSTATECOUNT   99
#endif
#ifndef STB_TEXTEDIT_UNDOCHARCOUNT
#define STB_TEXTEDIT_UNDOCHARCOUNT   999
#endif
#ifndef STB_TEXTEDIT_CHARTYPE
#define STB_TEXTEDIT_CHARTYPE        int
#endif
#ifndef STB_TEXTEDIT_POSITIONTYPE
#define STB_TEXTEDIT_POSITIONTYPE    int
#endif

typedef struct
{
   // private data
   STB_TEXTEDIT_POSITIONTYPE  where;
   short           insert_length;
   short           delete_length;
   short           char_storage;
} StbUndoRecord;

typedef struct
{
   // private data
   StbUndoRecord          undo_rec [STB_TEXTEDIT_UNDOSTATECOUNT];
   STB_TEXTEDIT_CHARTYPE  undo_char[STB_TEXTEDIT_UNDOCHARCOUNT];
   short undo_point, redo_point;
   short undo_char_point, redo_char_point;
} StbUndoState;

typedef struct
{
   /////////////////////
   //
   // public data
   //

   int cursor;
   // position of the text cursor within the string

   int select_start;          // selection start point
   int select_end;
   // selection start and end point in characters; if equal, no selection.
   // note that start may be less than or greater than end (e.g. when
   // dragging the mouse, start is where the initial click was, and you
   // can drag in either direction)

   unsigned char insert_mode;
   // each textfield keeps its own insert mode state. to keep an app-wide
   // insert mode, copy this value in/out of the app state

   /////////////////////
   //
   // private data
   //
   unsigned char cursor_at_end_of_line; // not implemented yet
   unsigned char initialized;
   unsigned char has_preferred_x;
   unsigned char single_line;
   unsigned char padding1, padding2, padding3;
   float preferred_x; // this determines where the cursor up/down tries to seek to along x
   StbUndoState undostate;
} STB_TexteditState;


////////////////////////////////////////////////////////////////////////
//
//     StbTexteditRow
//
// Result of layout query, used by stb_textedit to determine where
// the text in each row is.

// result of layout query
typedef struct
{
   float x0,x1;             // starting x location, end x location (allows for align=right, etc)
   float baseline_y_delta;  // position of baseline relative to previous row's baseline
   float ymin,ymax;         // height of row above and below baseline
   int num_chars;
} StbTexteditRow;
#endif //INCLUDE_STB_TEXTEDIT_H


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////
////   Implementation mode
////
////


// implementation isn't include-guarded, since it might have indirectly
// included just the "header" portion
#ifdef STB_TEXTEDIT_IMPLEMENTATION

#ifndef STB_TEXTEDIT_memmove
#include <string.h>
#define STB_TEXTEDIT_memmove memmove
#endif


/////////////////////////////////////////////////////////////////////////////
//
//      Mouse input handling
//

// traverse the layout to locate the nearest character to a display position
static int stb_text_locate_coord(STB_TEXTEDIT_STRING *str, float x, float y)
{
   StbTexteditRow r;
   int n = STB_TEXTEDIT_STRINGLEN(str);
   float base_y = 0, prev_x;
   int i=0, k;

   r.x0 = r.x1 = 0;
   r.ymin = r.ymax = 0;
   r.num_chars = 0;

   // search rows to find one that straddles 'y'
   while (i < n) {
      STB_TEXTEDIT_LAYOUTROW(&r, str, i);
      if (r.num_chars <= 0)
         return n;

      if (i==0 && y < base_y + r.ymin)
         return 0;

      if (y < base_y + r.ymax)
         break;

      i += r.num_chars;
      base_y += r.baseline_y_delta;
   }

   // below all text, return 'after' last character
   if (i >= n)
      return n;

   // check if it's before the beginning of the line
   if (x < r.x0)
      return i;

   // check if it's before the end of the line
   if (x < r.x1) {
      // search characters in row for one that straddles 'x'
      prev_x = r.x0;
      for (k=0; k < r.num_chars; ++k) {
         float w = STB_TEXTEDIT_GETWIDTH(str, i, k);
         if (x < prev_x+w) {
            if (x < prev_x+w/2)
               return k+i;
            else
               return k+i+1;
         }
         prev_x += w;
      }
      // shouldn't happen, but if it does, fall through to end-of-line case
   }

   // if the last character is a newline, return that. otherwise return 'after' the last character
   if (STB_TEXTEDIT_GETCHAR(str, i+r.num_chars-1) == STB_TEXTEDIT_NEWLINE)
      return i+r.num_chars-1;
   else
      return i+r.num_chars;
}

// API click: on mouse down, move the cursor to the clicked location, and reset the selection
static void stb_textedit_click(STB_TEXTEDIT_STRING *str, STB_TexteditState *state, float x, float y)
{
   state->cursor = stb_text_locate_coord(str, x, y);
   state->select_start = state->cursor;
   state->select_end = state->cursor;
   state->has_preferred_x = 0;
}

// API drag: on mouse drag, move the cursor and selection endpoint to the clicked location
static void stb_textedit_drag(STB_TEXTEDIT_STRING *str, STB_TexteditState *state, float x, float y)
{
   int p = stb_text_locate_coord(str, x, y);
   if (state->select_start == state->select_end)
      state->select_start = state->cursor;
   state->cursor = state->select_end = p;
}

/////////////////////////////////////////////////////////////////////////////
//
//      Keyboard input handling
//

// forward declarations
static void stb_text_undo(STB_TEXTEDIT_STRING *str, STB_TexteditState *state);
static void stb_text_redo(STB_TEXTEDIT_STRING *str, STB_TexteditState *state);
static void stb_text_makeundo_delete(STB_TEXTEDIT_STRING *str, STB_TexteditState *state, int where, int length);
static void stb_text_makeundo_insert(STB_TexteditState *state, int where, int length);
static void stb_text_makeundo_replace(STB_TEXTEDIT_STRING *str, STB_TexteditState *state, int where, int old_length, int new_length);

typedef struct
{
   float x,y;    // position of n'th character
   float height; // height of line
   int first_char, length; // first char of row, and length
   int prev_first;  // first char of previous row
} StbFindState;

// find the x/y location of a character, and remember info about the previous row in
// case we get a move-up event (for page up, we'll have to rescan)
static void stb_textedit_find_charpos(StbFindState *find, STB_TEXTEDIT_STRING *str, int n, int single_line)
{
   StbTexteditRow r;
   int prev_start = 0;
   int z = STB_TEXTEDIT_STRINGLEN(str);
   int i=0, first;

   if (n == z) {
      // if it's at the end, then find the last line -- simpler than trying to
      // explicitly handle this case in the regular code
      if (single_line) {
         STB_TEXTEDIT_LAYOUTROW(&r, str, 0);
         find->y = 0;
         find->first_char = 0;
         find->length = z;
         find->height = r.ymax - r.ymin;
         find->x = r.x1;
      } else {
         find->y = 0;
         find->x = 0;
         find->height = 1;
         while (i < z) {
            STB_TEXTEDIT_LAYOUTROW(&r, str, i);
            prev_start = i;
            i += r.num_chars;
         }
         find->first_char = i;
         find->length = 0;
         find->prev_first = prev_start;
      }
      return;
   }

   // search rows to find the one that straddles character n
   find->y = 0;

   for(;;) {
      STB_TEXTEDIT_LAYOUTROW(&r, str, i);
      if (n < i + r.num_chars)
         break;
      prev_start = i;
      i += r.num_chars;
      find->y += r.baseline_y_delta;
   }

   find->first_char = first = i;
   find->length = r.num_chars;
   find->height = r.ymax - r.ymin;
   find->prev_first = prev_start;

   // now scan to find xpos
   find->x = r.x0;
   i = 0;
   for (i=0; first+i < n; ++i)
      find->x += STB_TEXTEDIT_GETWIDTH(str, first, i);
}

#define STB_TEXT_HAS_SELECTION(s)   ((s)->select_start != (s)->select_end)

// make the selection/cursor state valid if client altered the string
static void stb_textedit_clamp(STB_TEXTEDIT_STRING *str, STB_TexteditState *state)
{
   int n = STB_TEXTEDIT_STRINGLEN(str);
   if (STB_TEXT_HAS_SELECTION(state)) {
      if (state->select_start > n) state->select_start = n;
      if (state->select_end   > n) state->select_end = n;
      // if clamping forced them to be equal, move the cursor to match
      if (state->select_start == state->select_end)
         state->cursor = state->select_start;
   }
   if (state->cursor > n) state->cursor = n;
}

// delete characters while updating undo
static void stb_textedit_delete(STB_TEXTEDIT_STRING *str, STB_TexteditState *state, int where, int len)
{
   stb_text_makeundo_delete(str, state, where, len);
   STB_TEXTEDIT_DELETECHARS(str, where, len);
   state->has_preferred_x = 0;
}

// delete the section
static void stb_textedit_delete_selection(STB_TEXTEDIT_STRING *str, STB_TexteditState *state)
{
   stb_textedit_clamp(str, state);
   if (STB_TEXT_HAS_SELECTION(state)) {
      if (state->select_start < state->select_end) {
         stb_textedit_delete(str, state, state->select_start, state->select_end - state->select_start);
         state->select_end = state->cursor = state->select_start;
      } else {
         stb_textedit_delete(str, state, state->select_end, state->select_start - state->select_end);
         state->select_start = state->cursor = state->select_end;
      }
      state->has_preferred_x = 0;
   }
}

// canoncialize the selection so start <= end
static void stb_textedit_sortselection(STB_TexteditState *state)
{
   if (state->select_end < state->select_start) {
      int temp = state->select_end;
      state->select_end = state->select_start;
      state->select_start = temp;
   }
}

// move cursor to first character of selection
static void stb_textedit_move_to_first(STB_TexteditState *state)
{
   if (STB_TEXT_HAS_SELECTION(state)) {
      stb_textedit_sortselection(state);
      state->cursor = state->select_start;
      state->select_end = state->select_start;
      state->has_preferred_x = 0;
   }
}

// move cursor to last character of selection
static void stb_textedit_move_to_last(STB_TEXTEDIT_STRING *str, STB_TexteditState *state)
{
   if (STB_TEXT_HAS_SELECTION(state)) {
      stb_textedit_sortselection(state);
      stb_textedit_clamp(str, state);
      state->cursor = state->select_end;
      state->select_start = state->select_end;
      state->has_preferred_x = 0;
   }
}

#ifdef STB_TEXTEDIT_IS_SPACE
static int is_word_boundary( STB_TEXTEDIT_STRING *str, int idx )
{
   return idx > 0 ? (STB_TEXTEDIT_IS_SPACE( STB_TEXTEDIT_GETCHAR(str,idx-1) ) && !STB_TEXTEDIT_IS_SPACE( STB_TEXTEDIT_GETCHAR(str, idx) ) ) : 1;
}

#ifndef STB_TEXTEDIT_MOVEWORDLEFT
static int stb_textedit_move_to_word_previous( STB_TEXTEDIT_STRING *str, int c )
{
   --c; // always move at least one character
   while( c >= 0 && !is_word_boundary( str, c ) )
      --c;

   if( c < 0 )
      c = 0;

   return c;
}
#define STB_TEXTEDIT_MOVEWORDLEFT stb_textedit_move_to_word_previous
#endif

#ifndef STB_TEXTEDIT_MOVEWORDRIGHT
static int stb_textedit_move_to_word_next( STB_TEXTEDIT_STRING *str, int c )
{
   const int len = STB_TEXTEDIT_STRINGLEN(str);
   ++c; // always move at least one character
   while( c < len && !is_word_boundary( str, c ) )
      ++c;

   if( c > len )
      c = len;

   return c;
}
#define STB_TEXTEDIT_MOVEWORDRIGHT stb_textedit_move_to_word_next
#endif

#endif

// update selection and cursor to match each other
static void stb_textedit_prep_selection_at_cursor(STB_TexteditState *state)
{
   if (!STB_TEXT_HAS_SELECTION(state))
      state->select_start = state->select_end = state->cursor;
   else
      state->cursor = state->select_end;
}

// API cut: delete selection
static int stb_textedit_cut(STB_TEXTEDIT_STRING *str, STB_TexteditState *state)
{
   if (STB_TEXT_HAS_SELECTION(state)) {
      stb_textedit_delete_selection(str,state); // implicity clamps
      state->has_preferred_x = 0;
      return 1;
   }
   return 0;
}

// API paste: replace existing selection with passed-in text
static int stb_textedit_paste(STB_TEXTEDIT_STRING *str, STB_TexteditState *state, STB_TEXTEDIT_CHARTYPE const *ctext, int len)
{
   STB_TEXTEDIT_CHARTYPE *text = (STB_TEXTEDIT_CHARTYPE *) ctext;
   // if there's a selection, the paste should delete it
   stb_textedit_clamp(str, state);
   stb_textedit_delete_selection(str,state);
   // try to insert the characters
   if (STB_TEXTEDIT_INSERTCHARS(str, state->cursor, text, len)) {
      stb_text_makeundo_insert(state, state->cursor, len);
      state->cursor += len;
      state->has_preferred_x = 0;
      return 1;
   }
   // remove the undo since we didn't actually insert the characters
   if (state->undostate.undo_point)
      --state->undostate.undo_point;
   return 0;
}

// API key: process a keyboard input
static void stb_textedit_key(STB_TEXTEDIT_STRING *str, STB_TexteditState *state, int key)
{
retry:
   switch (key) {
      default: {
         int c = STB_TEXTEDIT_KEYTOTEXT(key);
         if (c > 0) {
            STB_TEXTEDIT_CHARTYPE ch = (STB_TEXTEDIT_CHARTYPE) c;

            // can't add newline in single-line mode
            if (c == '\n' && state->single_line)
               break;

            if (state->insert_mode && !STB_TEXT_HAS_SELECTION(state) && state->cursor < STB_TEXTEDIT_STRINGLEN(str)) {
               stb_text_makeundo_replace(str, state, state->cursor, 1, 1);
               STB_TEXTEDIT_DELETECHARS(str, state->cursor, 1);
               if (STB_TEXTEDIT_INSERTCHARS(str, state->cursor, &ch, 1)) {
                  ++state->cursor;
                  state->has_preferred_x = 0;
               }
            } else {
               stb_textedit_delete_selection(str,state); // implicity clamps
               if (STB_TEXTEDIT_INSERTCHARS(str, state->cursor, &ch, 1)) {
                  stb_text_makeundo_insert(state, state->cursor, 1);
                  ++state->cursor;
                  state->has_preferred_x = 0;
               }
            }
         }
         break;
      }

#ifdef STB_TEXTEDIT_K_INSERT
      case STB_TEXTEDIT_K_INSERT:
         state->insert_mode = !state->insert_mode;
         break;
#endif
         
      case STB_TEXTEDIT_K_UNDO:
         stb_text_undo(str, state);
         state->has_preferred_x = 0;
         break;

      case STB_TEXTEDIT_K_REDO:
         stb_text_redo(str, state);
         state->has_preferred_x = 0;
         break;

      case STB_TEXTEDIT_K_LEFT:
         // if currently there's a selection, move cursor to start of selection
         if (STB_TEXT_HAS_SELECTION(state))
            stb_textedit_move_to_first(state);
         else 
            if (state->cursor > 0)
               --state->cursor;
         state->has_preferred_x = 0;
         break;

      case STB_TEXTEDIT_K_RIGHT:
         // if currently there's a selection, move cursor to end of selection
         if (STB_TEXT_HAS_SELECTION(state))
            stb_textedit_move_to_last(str, state);
         else
            ++state->cursor;
         stb_textedit_clamp(str, state);
         state->has_preferred_x = 0;
         break;

      case STB_TEXTEDIT_K_LEFT | STB_TEXTEDIT_K_SHIFT:
         stb_textedit_clamp(str, state);
         stb_textedit_prep_selection_at_cursor(state);
         // move selection left
         if (state->select_end > 0)
            --state->select_end;
         state->cursor = state->select_end;
         state->has_preferred_x = 0;
         break;

#ifdef STB_TEXTEDIT_MOVEWORDLEFT
      case STB_TEXTEDIT_K_WORDLEFT:
         if (STB_TEXT_HAS_SELECTION(state))
            stb_textedit_move_to_first(state);
         else {
            state->cursor = STB_TEXTEDIT_MOVEWORDLEFT(str, state->cursor);
            stb_textedit_clamp( str, state );
         }
         break;

      case STB_TEXTEDIT_K_WORDLEFT | STB_TEXTEDIT_K_SHIFT:
         if( !STB_TEXT_HAS_SELECTION( state ) )
            stb_textedit_prep_selection_at_cursor(state);

         state->cursor = STB_TEXTEDIT_MOVEWORDLEFT(str, state->cursor);
         state->select_end = state->cursor;

         stb_textedit_clamp( str, state );
         break;
#endif

#ifdef STB_TEXTEDIT_MOVEWORDRIGHT
      case STB_TEXTEDIT_K_WORDRIGHT:
         if (STB_TEXT_HAS_SELECTION(state)) 
            stb_textedit_move_to_last(str, state);
         else {
            state->cursor = STB_TEXTEDIT_MOVEWORDRIGHT(str, state->cursor);
            stb_textedit_clamp( str, state );
         }
         break;

      case STB_TEXTEDIT_K_WORDRIGHT | STB_TEXTEDIT_K_SHIFT:
         if( !STB_TEXT_HAS_SELECTION( state ) )
            stb_textedit_prep_selection_at_cursor(state);

         state->cursor = STB_TEXTEDIT_MOVEWORDRIGHT(str, state->cursor);
         state->select_end = state->cursor;

         stb_textedit_clamp( str, state );
         break;
#endif

      case STB_TEXTEDIT_K_RIGHT | STB_TEXTEDIT_K_SHIFT:
         stb_textedit_prep_selection_at_cursor(state);
         // move selection right
         ++state->select_end;
         stb_textedit_clamp(str, state);
         state->cursor = state->select_end;
         state->has_preferred_x = 0;
         break;

      case STB_TEXTEDIT_K_DOWN:
      case STB_TEXTEDIT_K_DOWN | STB_TEXTEDIT_K_SHIFT: {
         StbFindState find;
         StbTexteditRow row;
         int i, sel = (key & STB_TEXTEDIT_K_SHIFT) != 0;

         if (state->single_line) {
            // on windows, up&down in single-line behave like left&right
            key = STB_TEXTEDIT_K_RIGHT | (key & STB_TEXTEDIT_K_SHIFT);
            goto retry;
         }

         if (sel)
            stb_textedit_prep_selection_at_cursor(state);
         else if (STB_TEXT_HAS_SELECTION(state))
            stb_textedit_move_to_last(str,state);

         // compute current position of cursor point
         stb_textedit_clamp(str, state);
         stb_textedit_find_charpos(&find, str, state->cursor, state->single_line);

         // now find character position down a row
         if (find.length) {
            float goal_x = state->has_preferred_x ? state->preferred_x : find.x;
            float x;
            int start = find.first_char + find.length;
            state->cursor = start;
            STB_TEXTEDIT_LAYOUTROW(&row, str, state->cursor);
            x = row.x0;
            for (i=0; i < row.num_chars; ++i) {
               float dx = STB_TEXTEDIT_GETWIDTH(str, start, i);
               #ifdef STB_TEXTEDIT_GETWIDTH_NEWLINE
               if (dx == STB_TEXTEDIT_GETWIDTH_NEWLINE)
                  break;
               #endif
               x += dx;
               if (x > goal_x)
                  break;
               ++state->cursor;
            }
            stb_textedit_clamp(str, state);

            state->has_preferred_x = 1;
            state->preferred_x = goal_x;

            if (sel)
               state->select_end = state->cursor;
         }
         break;
      }
         
      case STB_TEXTEDIT_K_UP:
      case STB_TEXTEDIT_K_UP | STB_TEXTEDIT_K_SHIFT: {
         StbFindState find;
         StbTexteditRow row;
         int i, sel = (key & STB_TEXTEDIT_K_SHIFT) != 0;

         if (state->single_line) {
            // on windows, up&down become left&right
            key = STB_TEXTEDIT_K_LEFT | (key & STB_TEXTEDIT_K_SHIFT);
            goto retry;
         }

         if (sel)
            stb_textedit_prep_selection_at_cursor(state);
         else if (STB_TEXT_HAS_SELECTION(state))
            stb_textedit_move_to_first(state);

         // compute current position of cursor point
         stb_textedit_clamp(str, state);
         stb_textedit_find_charpos(&find, str, state->cursor, state->single_line);

         // can only go up if there's a previous row
         if (find.prev_first != find.first_char) {
            // now find character position up a row
            float goal_x = state->has_preferred_x ? state->preferred_x : find.x;
            float x;
            state->cursor = find.prev_first;
            STB_TEXTEDIT_LAYOUTROW(&row, str, state->cursor);
            x = row.x0;
            for (i=0; i < row.num_chars; ++i) {
               float dx = STB_TEXTEDIT_GETWIDTH(str, find.prev_first, i);
               #ifdef STB_TEXTEDIT_GETWIDTH_NEWLINE
               if (dx == STB_TEXTEDIT_GETWIDTH_NEWLINE)
                  break;
               #endif
               x += dx;
               if (x > goal_x)
                  break;
               ++state->cursor;
            }
            stb_textedit_clamp(str, state);

            state->has_preferred_x = 1;
            state->preferred_x = goal_x;

            if (sel)
               state->select_end = state->cursor;
         }
         break;
      }

      case STB_TEXTEDIT_K_DELETE:
      case STB_TEXTEDIT_K_DELETE | STB_TEXTEDIT_K_SHIFT:
         if (STB_TEXT_HAS_SELECTION(state))
            stb_textedit_delete_selection(str, state);
         else {
            int n = STB_TEXTEDIT_STRINGLEN(str);
            if (state->cursor < n)
               stb_textedit_delete(str, state, state->cursor, 1);
         }
         state->has_preferred_x = 0;
         break;

      case STB_TEXTEDIT_K_BACKSPACE:
      case STB_TEXTEDIT_K_BACKSPACE | STB_TEXTEDIT_K_SHIFT:
         if (STB_TEXT_HAS_SELECTION(state))
            stb_textedit_delete_selection(str, state);
         else {
            stb_textedit_clamp(str, state);
            if (state->cursor > 0) {
               stb_textedit_delete(str, state, state->cursor-1, 1);
               --state->cursor;
            }
         }
         state->has_preferred_x = 0;
         break;
         
#ifdef STB_TEXTEDIT_K_TEXTSTART2
      case STB_TEXTEDIT_K_TEXTSTART2:
#endif
      case STB_TEXTEDIT_K_TEXTSTART:
         state->cursor = state->select_start = state->select_end = 0;
         state->has_preferred_x = 0;
         break;

#ifdef STB_TEXTEDIT_K_TEXTEND2
      case STB_TEXTEDIT_K_TEXTEND2:
#endif
      case STB_TEXTEDIT_K_TEXTEND:
         state->cursor = STB_TEXTEDIT_STRINGLEN(str);
         state->select_start = state->select_end = 0;
         state->has_preferred_x = 0;
         break;
        
#ifdef STB_TEXTEDIT_K_TEXTSTART2
      case STB_TEXTEDIT_K_TEXTSTART2 | STB_TEXTEDIT_K_SHIFT:
#endif
      case STB_TEXTEDIT_K_TEXTSTART | STB_TEXTEDIT_K_SHIFT:
         stb_textedit_prep_selection_at_cursor(state);
         state->cursor = state->select_end = 0;
         state->has_preferred_x = 0;
         break;

#ifdef STB_TEXTEDIT_K_TEXTEND2
      case STB_TEXTEDIT_K_TEXTEND2 | STB_TEXTEDIT_K_SHIFT:
#endif
      case STB_TEXTEDIT_K_TEXTEND | STB_TEXTEDIT_K_SHIFT:
         stb_textedit_prep_selection_at_cursor(state);
         state->cursor = state->select_end = STB_TEXTEDIT_STRINGLEN(str);
         state->has_preferred_x = 0;
         break;


#ifdef STB_TEXTEDIT_K_LINESTART2
      case STB_TEXTEDIT_K_LINESTART2:
#endif
      case STB_TEXTEDIT_K_LINESTART:
         stb_textedit_clamp(str, state);
         stb_textedit_move_to_first(state);
         if (state->single_line)
            state->cursor = 0;
         else while (state->cursor > 0 && STB_TEXTEDIT_GETCHAR(str, state->cursor-1) != STB_TEXTEDIT_NEWLINE)
            --state->cursor;
         state->has_preferred_x = 0;
         break;

#ifdef STB_TEXTEDIT_K_LINEEND2
      case STB_TEXTEDIT_K_LINEEND2:
#endif
      case STB_TEXTEDIT_K_LINEEND: {
         int n = STB_TEXTEDIT_STRINGLEN(str);
         stb_textedit_clamp(str, state);
         stb_textedit_move_to_first(state);
         if (state->single_line)
             state->cursor = n;
         else while (state->cursor < n && STB_TEXTEDIT_GETCHAR(str, state->cursor) != STB_TEXTEDIT_NEWLINE)
             ++state->cursor;
         state->has_preferred_x = 0;
         break;
      }

#ifdef STB_TEXTEDIT_K_LINESTART2
      case STB_TEXTEDIT_K_LINESTART2 | STB_TEXTEDIT_K_SHIFT:
#endif
      case STB_TEXTEDIT_K_LINESTART | STB_TEXTEDIT_K_SHIFT:
         stb_textedit_clamp(str, state);
         stb_textedit_prep_selection_at_cursor(state);
         if (state->single_line)
            state->cursor = 0;
         else while (state->cursor > 0 && STB_TEXTEDIT_GETCHAR(str, state->cursor-1) != STB_TEXTEDIT_NEWLINE)
            --state->cursor;
         state->select_end = state->cursor;
         state->has_preferred_x = 0;
         break;

#ifdef STB_TEXTEDIT_K_LINEEND2
      case STB_TEXTEDIT_K_LINEEND2 | STB_TEXTEDIT_K_SHIFT:
#endif
      case STB_TEXTEDIT_K_LINEEND | STB_TEXTEDIT_K_SHIFT: {
         int n = STB_TEXTEDIT_STRINGLEN(str);
         stb_textedit_clamp(str, state);
         stb_textedit_prep_selection_at_cursor(state);
         if (state->single_line)
             state->cursor = n;
         else while (state->cursor < n && STB_TEXTEDIT_GETCHAR(str, state->cursor) != STB_TEXTEDIT_NEWLINE)
            ++state->cursor;
         state->select_end = state->cursor;
         state->has_preferred_x = 0;
         break;
      }

// @TODO:
//    STB_TEXTEDIT_K_PGUP      - move cursor up a page
//    STB_TEXTEDIT_K_PGDOWN    - move cursor down a page
   }
}

/////////////////////////////////////////////////////////////////////////////
//
//      Undo processing
//
// @OPTIMIZE: the undo/redo buffer should be circular

static void stb_textedit_flush_redo(StbUndoState *state)
{
   state->redo_point = STB_TEXTEDIT_UNDOSTATECOUNT;
   state->redo_char_point = STB_TEXTEDIT_UNDOCHARCOUNT;
}

// discard the oldest entry in the undo list
static void stb_textedit_discard_undo(StbUndoState *state)
{
   if (state->undo_point > 0) {
      // if the 0th undo state has characters, clean those up
      if (state->undo_rec[0].char_storage >= 0) {
         int n = state->undo_rec[0].insert_length, i;
         // delete n characters from all other records
         state->undo_char_point = state->undo_char_point - (short) n;  // vsnet05
         STB_TEXTEDIT_memmove(state->undo_char, state->undo_char + n, (size_t) ((size_t)state->undo_char_point*sizeof(STB_TEXTEDIT_CHARTYPE)));
         for (i=0; i < state->undo_point; ++i)
            if (state->undo_rec[i].char_storage >= 0)
               state->undo_rec[i].char_storage = state->undo_rec[i].char_storage - (short) n; // vsnet05 // @OPTIMIZE: get rid of char_storage and infer it
      }
      --state->undo_point;
      STB_TEXTEDIT_memmove(state->undo_rec, state->undo_rec+1, (size_t) ((size_t)state->undo_point*sizeof(state->undo_rec[0])));
   }
}

// discard the oldest entry in the redo list--it's bad if this
// ever happens, but because undo & redo have to store the actual
// characters in different cases, the redo character buffer can
// fill up even though the undo buffer didn't
static void stb_textedit_discard_redo(StbUndoState *state)
{
   int k = STB_TEXTEDIT_UNDOSTATECOUNT-1;

   if (state->redo_point <= k) {
      // if the k'th undo state has characters, clean those up
      if (state->undo_rec[k].char_storage >= 0) {
         int n = state->undo_rec[k].insert_length, i;
         // delete n characters from all other records
         state->redo_char_point = state->redo_char_point + (short) n; // vsnet05
         STB_TEXTEDIT_memmove(state->undo_char + state->redo_char_point, state->undo_char + state->redo_char_point-n, (size_t) ((size_t)(STB_TEXTEDIT_UNDOCHARCOUNT - state->redo_char_point)*sizeof(STB_TEXTEDIT_CHARTYPE)));
         for (i=state->redo_point; i < k; ++i)
            if (state->undo_rec[i].char_storage >= 0)
               state->undo_rec[i].char_storage = state->undo_rec[i].char_storage + (short) n; // vsnet05
      }
      STB_TEXTEDIT_memmove(state->undo_rec + state->redo_point, state->undo_rec + state->redo_point-1, (size_t) ((size_t)(STB_TEXTEDIT_UNDOSTATECOUNT - state->redo_point)*sizeof(state->undo_rec[0])));
      ++state->redo_point;
   }
}

static StbUndoRecord *stb_text_create_undo_record(StbUndoState *state, int numchars)
{
   // any time we create a new undo record, we discard redo
   stb_textedit_flush_redo(state);

   // if we have no free records, we have to make room, by sliding the
   // existing records down
   if (state->undo_point == STB_TEXTEDIT_UNDOSTATECOUNT)
      stb_textedit_discard_undo(state);

   // if the characters to store won't possibly fit in the buffer, we can't undo
   if (numchars > STB_TEXTEDIT_UNDOCHARCOUNT) {
      state->undo_point = 0;
      state->undo_char_point = 0;
      return NULL;
   }

   // if we don't have enough free characters in the buffer, we have to make room
   while (state->undo_char_point + numchars > STB_TEXTEDIT_UNDOCHARCOUNT)
      stb_textedit_discard_undo(state);

   return &state->undo_rec[state->undo_point++];
}

static STB_TEXTEDIT_CHARTYPE *stb_text_createundo(StbUndoState *state, int pos, int insert_len, int delete_len)
{
   StbUndoRecord *r = stb_text_create_undo_record(state, insert_len);
   if (r == NULL)
      return NULL;

   r->where = pos;
   r->insert_length = (short) insert_len;
   r->delete_length = (short) delete_len;

   if (insert_len == 0) {
      r->char_storage = -1;
      return NULL;
   } else {
      r->char_storage = state->undo_char_point;
      state->undo_char_point = state->undo_char_point + (short) insert_len;
      return &state->undo_char[r->char_storage];
   }
}

static void stb_text_undo(STB_TEXTEDIT_STRING *str, STB_TexteditState *state)
{
   StbUndoState *s = &state->undostate;
   StbUndoRecord u, *r;
   if (s->undo_point == 0)
      return;

   // we need to do two things: apply the undo record, and create a redo record
   u = s->undo_rec[s->undo_point-1];
   r = &s->undo_rec[s->redo_point-1];
   r->char_storage = -1;

   r->insert_length = u.delete_length;
   r->delete_length = u.insert_length;
   r->where = u.where;

   if (u.delete_length) {
      // if the undo record says to delete characters, then the redo record will
      // need to re-insert the characters that get deleted, so we need to store
      // them.

      // there are three cases:
      //    there's enough room to store the characters
      //    characters stored for *redoing* don't leave room for redo
      //    characters stored for *undoing* don't leave room for redo
      // if the last is true, we have to bail

      if (s->undo_char_point + u.delete_length >= STB_TEXTEDIT_UNDOCHARCOUNT) {
         // the undo records take up too much character space; there's no space to store the redo characters
         r->insert_length = 0;
      } else {
         int i;

         // there's definitely room to store the characters eventually
         while (s->undo_char_point + u.delete_length > s->redo_char_point) {
            // there's currently not enough room, so discard a redo record
            stb_textedit_discard_redo(s);
            // should never happen:
            if (s->redo_point == STB_TEXTEDIT_UNDOSTATECOUNT)
               return;
         }
         r = &s->undo_rec[s->redo_point-1];

         r->char_storage = s->redo_char_point - u.delete_length;
         s->redo_char_point = s->redo_char_point - (short) u.delete_length;

         // now save the characters
         for (i=0; i < u.delete_length; ++i)
            s->undo_char[r->char_storage + i] = STB_TEXTEDIT_GETCHAR(str, u.where + i);
      }

      // now we can carry out the deletion
      STB_TEXTEDIT_DELETECHARS(str, u.where, u.delete_length);
   }

   // check type of recorded action:
   if (u.insert_length) {
      // easy case: was a deletion, so we need to insert n characters
      STB_TEXTEDIT_INSERTCHARS(str, u.where, &s->undo_char[u.char_storage], u.insert_length);
      s->undo_char_point -= u.insert_length;
   }

   state->cursor = u.where + u.insert_length;

   s->undo_point--;
   s->redo_point--;
}

static void stb_text_redo(STB_TEXTEDIT_STRING *str, STB_TexteditState *state)
{
   StbUndoState *s = &state->undostate;
   StbUndoRecord *u, r;
   if (s->redo_point == STB_TEXTEDIT_UNDOSTATECOUNT)
      return;

   // we need to do two things: apply the redo record, and create an undo record
   u = &s->undo_rec[s->undo_point];
   r = s->undo_rec[s->redo_point];

   // we KNOW there must be room for the undo record, because the redo record
   // was derived from an undo record

   u->delete_length = r.insert_length;
   u->insert_length = r.delete_length;
   u->where = r.where;
   u->char_storage = -1;

   if (r.delete_length) {
      // the redo record requires us to delete characters, so the undo record
      // needs to store the characters

      if (s->undo_char_point + u->insert_length > s->redo_char_point) {
         u->insert_length = 0;
         u->delete_length = 0;
      } else {
         int i;
         u->char_storage = s->undo_char_point;
         s->undo_char_point = s->undo_char_point + u->insert_length;

         // now save the characters
         for (i=0; i < u->insert_length; ++i)
            s->undo_char[u->char_storage + i] = STB_TEXTEDIT_GETCHAR(str, u->where + i);
      }

      STB_TEXTEDIT_DELETECHARS(str, r.where, r.delete_length);
   }

   if (r.insert_length) {
      // easy case: need to insert n characters
      STB_TEXTEDIT_INSERTCHARS(str, r.where, &s->undo_char[r.char_storage], r.insert_length);
      s->redo_char_point += r.insert_length;
   }

   state->cursor = r.where + r.insert_length;

   s->undo_point++;
   s->redo_point++;
}

static void stb_text_makeundo_insert(STB_TexteditState *state, int where, int length)
{
   stb_text_createundo(&state->undostate, where, 0, length);
}

static void stb_text_makeundo_delete(STB_TEXTEDIT_STRING *str, STB_TexteditState *state, int where, int length)
{
   int i;
   STB_TEXTEDIT_CHARTYPE *p = stb_text_createundo(&state->undostate, where, length, 0);
   if (p) {
      for (i=0; i < length; ++i)
         p[i] = STB_TEXTEDIT_GETCHAR(str, where+i);
   }
}

static void stb_text_makeundo_replace(STB_TEXTEDIT_STRING *str, STB_TexteditState *state, int where, int old_length, int new_length)
{
   int i;
   STB_TEXTEDIT_CHARTYPE *p = stb_text_createundo(&state->undostate, where, old_length, new_length);
   if (p) {
      for (i=0; i < old_length; ++i)
         p[i] = STB_TEXTEDIT_GETCHAR(str, where+i);
   }
}

// reset the state to default
static void stb_textedit_clear_state(STB_TexteditState *state, int is_single_line)
{
   state->undostate.undo_point = 0;
   state->undostate.undo_char_point = 0;
   state->undostate.redo_point = STB_TEXTEDIT_UNDOSTATECOUNT;
   state->undostate.redo_char_point = STB_TEXTEDIT_UNDOCHARCOUNT;
   state->select_end = state->select_start = 0;
   state->cursor = 0;
   state->has_preferred_x = 0;
   state->preferred_x = 0;
   state->cursor_at_end_of_line = 0;
   state->initialized = 1;
   state->single_line = (unsigned char) is_single_line;
   state->insert_mode = 0;
}

// API initialize
static void stb_textedit_initialize_state(STB_TexteditState *state, int is_single_line)
{
   stb_textedit_clear_state(state, is_single_line);
}
#endif//STB_TEXTEDIT_IMPLEMENTATION
