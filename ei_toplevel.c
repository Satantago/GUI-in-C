#include "ei_toplevel.h"
#include "ei_widget.h"
#include "stdlib.h"
#include "ei_frame.h"
#include "ei_draw.h"
#include "hw_interface.h"
#include "ei_utils.h"
#include "ei_button.h"
#include "math.h"
#include "ei_application.h"
#include "ei_geometrymanager.h"

// alloue de l'espace mémoire pour un frame

struct ei_widget_t* toplevel_allocfunc_t(void)
{
    ei_frame_t *top_level = calloc(1, sizeof(ei_frame_t));
    return (ei_widget_t*)top_level;
}
// on libère les attributs de frame
void toplevel_releasefunc_t(struct ei_widget_t * widget)
{
    free(((ei_toplevel_t *)widget)->title);
    free(((ei_toplevel_t *)widget)->min_size);
}

void *bouton_ferme(ei_widget_t *widget, ei_surface_t surface, ei_surface_t pick_surface, ei_surface_t clipper) {

    ei_color_t red={150,0,0,0xff};
	int		button_border_width	= 0;
    int hauteur_ruban = hw_surface_get_size(hw_text_create_surface(((ei_toplevel_t*)widget)->title, ei_default_font, ei_font_default_color)).height;

    int rayon_bouton = hauteur_ruban/3;
    ei_point_t centre = ei_point_sub( ei_point_zero(), ei_point(-0.5*rayon_bouton, 2.5*rayon_bouton)) ;
    ei_callback_t button_callback = quit;
    ei_size_t size = ei_size(2*rayon_bouton,2*rayon_bouton);
    ei_widget_t  *my_bouton = ei_widget_create("button",widget,NULL,NULL);
    ei_button_configure(my_bouton,&size,&red,&button_border_width,&rayon_bouton,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
                        &button_callback,NULL);
    ei_place(my_bouton, NULL, &centre.x, &centre.y, NULL, NULL, NULL, NULL, NULL, NULL );
}


void    toplevel_drawfunc_t	(struct ei_widget_t *	widget,
                                ei_surface_t		surface,
                                ei_surface_t		pick_surface,
                                ei_rect_t*		clipper)
{
    if ((widget->geom_params != NULL && widget != ei_app_root_widget()) || widget == ei_app_root_widget())

    {
        ei_point_t point_depart;
        int hauteur_ruban = hw_surface_get_size(hw_text_create_surface(((ei_toplevel_t*)widget)->title, ei_default_font, ei_font_default_color)).height;


        int rayon_ruban = hauteur_ruban/2;
        ei_point_t centre_ruban = ei_point_add(widget->screen_location.top_left, ei_point(rayon_ruban, rayon_ruban));


        ei_color_t red={0xff,0,0,0xff};
        ei_point_t top_left = widget->screen_location.top_left;
        point_depart = widget->content_rect->top_left  ;
        int largeur      = widget->requested_size.width;
        int hauteur      = widget->requested_size.height;

        ei_point_t point = ei_point(top_left.x+largeur/4,top_left.y);
        //traçage du cadre arrondi
        ei_color_t color={0x88,0x88,0x88,0xff};



        ei_point_t centre1;


        centre1 = ei_point_add(top_left, ei_point(
                largeur+2*((ei_toplevel_t*)widget)->border_width-rayon_ruban, rayon_ruban));

        ei_linked_point_t *arc_top_left  = arc(centre_ruban, rayon_ruban, M_PI / 2, M_PI);
        ei_linked_point_t *arc_top_right = arc(centre1, rayon_ruban, 0, M_PI / 2);

        queue(arc_top_right)->next = arc_top_left;
        inserer_queue(&arc_top_left, top_left.x, hauteur_ruban + ((ei_toplevel_t*)widget)->border_width
        + hauteur+top_left.y);
        inserer_queue(&arc_top_left, top_left.x+2*((ei_toplevel_t*)widget)->border_width+largeur,
                      top_left.y+hauteur_ruban+hauteur+((ei_toplevel_t*)widget)->border_width);
        ei_draw_polygon(surface, arc_top_right, color, clipper);
        ei_draw_polygon(pick_surface, arc_top_right, *widget->pick_color, clipper);

        //Traçage du rectangle du milieu
        ei_linked_point_t  points1[4] ;
        points1[0].point.x	= point_depart.x ;
        points1[0].point.y	= point_depart.y ;
        points1[0].next = &points1[1];

        points1[1].point.x = points1[0].point.x + largeur;
        points1[1].point.y = points1[0].point.y;
        points1[1].next = &points1[2];

        points1[2].point.x = points1[1].point.x;
        points1[2].point.y = points1[1].point.y + hauteur;
        points1[2].next = &points1[3];

        points1[3].point.x = points1[0].point.x;
        points1[3].point.y = points1[2].point.y;
        points1[3].next = NULL;

        ei_draw_polygon(surface,points1,((ei_toplevel_t*)widget)->color,clipper);


        if (((ei_toplevel_t*)widget)->closable == EI_TRUE)
        {
            bouton_ferme(widget,surface,pick_surface,clipper);
        }
        if (((ei_toplevel_t*)widget)->title)
        {
            ei_draw_text(surface, &point, ((ei_toplevel_t*)widget)->title,
                      ei_default_font, ei_font_default_color, &widget->screen_location);
        }
        // pour le carre de deplacement on supposera que il est de meme taille que le bouton quitter

        ei_point_t point_depart_carre = ei_point_add(top_left,
                                                     ei_point(largeur+2*((ei_toplevel_t*)widget)->border_width,
                                                      hauteur+hauteur_ruban+((ei_toplevel_t*)widget)->border_width));
        ei_linked_point_t  square[4] ;
        square[0].point.x	= point_depart_carre.x ;
        square[0].point.y	= point_depart_carre.y ;
        square[0].next = &square[1];

        square[1].point.x = square[0].point.x ;
        square[1].point.y = square[0].point.y - rayon_ruban;
        square[1].next = &square[2];

        square[2].point.x = square[1].point.x - rayon_ruban;
        square[2].point.y = square[1].point.y ;
        square[2].next = &square[3];

        square[3].point.x = square[2].point.x;
        square[3].point.y = square[0].point.y;
        square[3].next = NULL;

        ei_draw_polygon(surface,square, color,clipper);
    }
}

void toplevel_setdefaultsfunc_t	(struct ei_widget_t *widget)
{
    ((ei_toplevel_t*)widget)->color          = ei_default_background_color;
    ((ei_toplevel_t*)widget)->border_width   = 4;
    ((ei_toplevel_t*)widget)->title          = ei_default_font;
    ((ei_toplevel_t*)widget)->closable       = EI_TRUE;
    ((ei_toplevel_t*)widget)->resizable      = ei_axis_both;
    ((ei_toplevel_t*)widget)->requested_size = ei_size(320,240);
    widget->requested_size                   = ei_size_zero()                        ;
    widget->screen_location.top_left         = widget->parent->content_rect->top_left;
    widget->content_rect                     = malloc(sizeof (ei_rect_t))       ;
    *(widget->content_rect)                  = widget->screen_location               ;
}



