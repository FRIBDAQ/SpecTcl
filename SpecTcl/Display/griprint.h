#include "printer.h"

/*
** Module private classes:
*/

class PrintSpectrumDialog : public XMCustomDialog
{
protected:
  /* Creates the tabs */
  XMTab   *tab;
  XMForm  *first_tab;
  XMForm  *second_tab;
  XMForm  *third_tab;
  XMForm  *fourth_tab;

  /* These are used in the first tab */
  XMFrame        *layout_frame;
  XMFrame        *spp_frame;
  XMFrame        *size_frame;
  XMRowColumn    *type_manager;
  XMRowColumn    *num_manager;
  XMRowColumn    *spp_mgr;
  XMRowColumn    *size_mgr;
  XMLabel        *rows;
  XMLabel        *cols;
  XMText         *t_rows;
  XMText         *t_cols;
  XMToggleButton *port;
  XMToggleButton *land;
  XMToggleButton *print_sel;
  XMToggleButton *print_all;
  XMToggleButton *ten_by_ten;
  XMToggleButton *ten_by_fifteen;
  XMToggleButton *user_specified;

  /* These are used in the second tab */
  XMRowColumn    *row_one;
  XMRowColumn    *row_two;
  XMRowColumn    *row_three;
  XMRowColumn    *row_four;
  XMRowColumn    *row_five;
  XMRowColumn    *row_six;
  XMRowColumn    *drawing_mgr;
  XMLabel        *spec_title;
  XMLabel        *name_x;
  XMLabel        *name_y;
  XMLabel        *length_x;
  XMLabel        *length_y;
  XMLabel        *contour_inc;
  XMText         *t_title;
  XMText         *t_name_x;
  XMText         *t_name_y;
  XMText         *t_length_x;
  XMText         *t_length_y;
  XMText         *t_contour_inc;
  XMToggleButton *draw_palette;
  XMToggleButton *draw_contours;
  XMToggleButton *draw_time;
  XMToggleButton *use_symbols;

  /* These are used in the third tab */
  XMFrame        *tics_frame;
  XMFrame        *tics_entry_frame;
  XMRowColumn    *tics_mgr;
  XMRowColumn    *tics_entry_mgr;
  XMLabel        *xmajor;
  XMLabel        *xminor;
  XMLabel        *ymajor;
  XMLabel        *yminor;
  XMText         *t_xmajor;
  XMText         *t_xminor;
  XMText         *t_ymajor;
  XMText         *t_yminor;
  XMToggleButton *default_tics;
  XMToggleButton *user_tics;

  /* These are used in the fourth tab */
  XMFrame        *dest_frame;
  XMFrame        *res_frame;
  XMRowColumn    *dest_mgr;
  XMRowColumn    *file_mgr;
  XMRowColumn    *cmd_mgr;
  XMRowColumn    *res_mgr;
  XMText         *t_file;
  XMText         *t_cmd;
  XMLabel        *cmd;
  XMLabel        *file;
  XMLabel        *res;
  XMScrolledList *list_type;
  XMToggleButton *to_file;
  XMToggleButton *to_printer;
  XMToggleButton *one_to_one;
  XMToggleButton *one_to_two;
  XMToggleButton *one_to_four;
  XMToggleButton *one_to_eight;

public:
  PrintSpectrumDialog(char *name, XMWidget *w, char *title);
  ~PrintSpectrumDialog() {
    /* The tab widgets */
    delete tab; tab = NULL;
    delete first_tab; first_tab = NULL;
    delete second_tab; second_tab = NULL;
    delete third_tab; third_tab = NULL;
    delete fourth_tab; fourth_tab = NULL;

    /* The page setup widgets */
    delete layout_frame; layout_frame = NULL;
    delete spp_frame; spp_frame = NULL;
    delete size_frame; size_frame = NULL;
    delete type_manager; type_manager = NULL;
    delete num_manager; num_manager = NULL;
    delete spp_mgr; spp_mgr = NULL;
    delete size_mgr; size_mgr = NULL;
    delete rows; rows = NULL;
    delete cols; cols = NULL;
    delete t_rows; t_rows = NULL;
    delete t_cols; t_cols = NULL;
    delete port; port = NULL;
    delete land; land = NULL; 
    delete print_sel; print_sel = NULL;
    delete print_all; print_all = NULL;
    delete ten_by_ten; ten_by_ten = NULL;
    delete ten_by_fifteen; ten_by_fifteen = NULL;
    delete user_specified; user_specified = NULL;

    /* The spectrum options widgets */
    delete tics_frame; tics_frame = NULL;
    delete tics_entry_frame; tics_entry_frame = NULL;
    delete row_one; row_one = NULL;
    delete row_two; row_two = NULL;
    delete row_three; row_three = NULL;
    delete row_four; row_four = NULL;
    delete row_five; row_five = NULL;
    delete row_six; row_six = NULL;
    delete drawing_mgr; drawing_mgr = NULL;
    delete tics_mgr; tics_mgr = NULL;
    delete tics_entry_mgr; tics_entry_mgr = NULL;
    delete spec_title;  spec_title = NULL;
    delete name_x; name_x = NULL;
    delete name_y; name_y = NULL;
    delete length_x; length_x = NULL;
    delete length_y; length_y = NULL;
    delete contour_inc; contour_inc = NULL;
    delete xmajor; xmajor = NULL;
    delete xminor; xminor = NULL;
    delete ymajor; ymajor = NULL;
    delete yminor; yminor = NULL;
    delete t_title; t_title = NULL;
    delete t_name_x; t_name_x = NULL;
    delete t_name_y; t_name_y = NULL;
    delete t_length_x; t_length_x = NULL;
    delete t_length_y; t_length_y = NULL;
    delete t_contour_inc; t_contour_inc = NULL;
    delete t_xmajor; t_xmajor = NULL;
    delete t_xminor; t_xminor = NULL;
    delete t_ymajor; t_ymajor = NULL;
    delete t_yminor; t_yminor = NULL;
    delete draw_palette; draw_palette = NULL;
    delete draw_contours; draw_contours = NULL;
    delete draw_time; draw_time = NULL;
    delete use_symbols; use_symbols = NULL;
    delete default_tics; default_tics = NULL;
    delete user_tics; user_tics = NULL;

    /* The output option widgets */
    delete dest_frame; dest_frame = NULL;
    delete res_frame; res_frame = NULL;
    delete dest_mgr; dest_mgr = NULL;
    delete file_mgr; file_mgr = NULL;
    delete cmd_mgr; cmd_mgr = NULL;
    delete res_mgr; res_mgr = NULL;
    delete t_file; t_file = NULL;
    delete t_cmd; t_cmd = NULL;
    delete cmd; cmd = NULL;
    delete file; file = NULL;
    delete res; res = NULL;
    delete list_type; list_type = NULL;
    delete to_file; to_file = NULL;
    delete to_printer; to_printer = NULL;
    delete one_to_one; one_to_one = NULL;
    delete one_to_two; one_to_two = NULL;
    delete one_to_four; one_to_four = NULL;
    delete one_to_eight; one_to_eight = NULL;
  }


  PrintLayout getlayout();
  void setlayout(PrintLayout layout);

  PrintNum getnum();
  void setnum(PrintNum num);

  PrintDest getdest();
  void setdest(PrintDest pd);

  Resolution getres();
  void setres(Resolution res);

  Tics gettics();
  void settics(Tics t);

  bool getdraw_palette() { return draw_palette->GetState(); }
  bool getdraw_contours() { return draw_contours->GetState(); }
  bool getdraw_time() { return draw_time->GetState(); }
  bool getuse_symbols() { return use_symbols->GetState(); }
  void setdraw_palette(bool);
  void setdraw_contours(bool);
  void setdraw_time(bool);
  void setuse_symbols(bool);
  void setcontour_val(bool);
  void set_select_res(bool);
  void allow_geom(bool);
  void allow_setsize(bool);
  void allow_tics(bool);

  char* getrows();
  void setrows(char* r);
  char* getcols();
  void setcols(char* c);
  char* gettitle();
  void settitle(char* title);
  char* getxname();
  void setxname(char* xname);
  char* getyname();
  void setyname(char* yname);
  float getxlen();
  void setxlen(char* xlen);
  float getylen();
  void setylen(char* ylen);
  char* getcontour_inc();
  void setcontour_inc(char* inc);
  double getxmajor();
  void setxmajor(char* maj);
  double getxminor();
  void setxminor(char* min);
  double getymajor();
  void setymajor(char* maj);
  double getyminor();
  void setyminor(char* min);
  void setcmd(char* prtcmd);
  char* getfile();
  void setfile(char* f);
  char* gettype();
  char* getcmd();
  void setspecified();

  /* Visibility control */

  void Manage() {
    /* Tabs widgets */
    tab->Manage();
    first_tab->Manage();
    second_tab->Manage();
    third_tab->Manage();
    fourth_tab->Manage();

    /* Page setup widgets */
    layout_frame->Manage();
    spp_frame->Manage();
    size_frame->Manage();
    type_manager->Manage();
    num_manager->Manage();
    spp_mgr->Manage();
    size_mgr->Manage();

    /* Spectrum options widgets */
    row_one->Manage();
    row_two->Manage();
    row_three->Manage();
    row_four->Manage();
    row_five->Manage();
    row_six->Manage();
    drawing_mgr->Manage();
    tics_frame->Manage();
    tics_entry_frame->Manage();
    tics_mgr->Manage();
    tics_entry_mgr->Manage();

    /* Output option widgets */
    dest_frame->Manage();
    res_frame->Manage();
    dest_mgr->Manage();
    file_mgr->Manage();
    cmd_mgr->Manage();
    res_mgr->Manage();

    XMCustomDialog::Manage();
  }

  void UnManage() {
    /* Tabs widgets */
    tab->UnManage();
    first_tab->UnManage();
    second_tab->UnManage();
    third_tab->UnManage();
    fourth_tab->UnManage();

    /* Page setup widgets */
    layout_frame->UnManage();
    spp_frame->UnManage();
    size_frame->UnManage();
    type_manager->UnManage();
    num_manager->UnManage();
    spp_mgr->UnManage();
    size_mgr->UnManage();

    /* Spectrum options widgets */
    row_one->UnManage();
    row_two->UnManage();
    row_three->UnManage();
    row_four->UnManage();
    row_five->UnManage();
    row_six->UnManage();
    drawing_mgr->UnManage();
    tics_frame->UnManage();
    tics_entry_frame->UnManage();
    tics_mgr->UnManage();
    tics_entry_mgr->UnManage();

    /* Output option widgets */
    dest_frame->UnManage();
    res_frame->UnManage();
    dest_mgr->UnManage();
    file_mgr->UnManage();
    cmd_mgr->UnManage();
    res_mgr->UnManage();

    XMCustomDialog::UnManage();
  }
};

