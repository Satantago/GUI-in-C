#include "ei_widget.h"
#include <stdlib.h>
#include "ei_frame.h"
#include "ei_draw.h"
#include "hw_interface.h"
#include "ei_application.h"
#include "ei_types.h"
#include "ei_utils.h"
#include "ei_button.h"
#include "ei_toplevel.h"
#include "ei_geometrymanager.h"


ei_rect_t intersection(ei_rect_t premier_rect , ei_rect_t deux_rect);

struct ei_widget_t* frame_allocfunc_t(void)
{
    ei_frame_t *frame = calloc(1, sizeof(ei_frame_t));
    return (ei_widget_t*)frame;
}

void frame_releasefunc_t(struct ei_widget_t * frame)
{
    free(((ei_frame_t*)frame)->text);
    free(((ei_frame_t*)frame)->img);
    free(frame);
}

void    frame_drawfunc_t	(struct ei_widget_t *	frame,
							 ei_surface_t		surface,
							 ei_surface_t		pick_surface,
							 ei_rect_t*		clipper)
{
    // draw the widget ( frame ) only if it is managed by a geometry manager
    if ((frame->geom_params != NULL && frame != ei_app_root_widget()))
    {
        ei_color_t color_bas;
        ei_color_t color_haut;

        // Relief

       switch (((ei_frame_t *)frame)->relief)
        {
            case  ei_relief_raised:
                color_bas = foncer_couleur(((ei_frame_t *)frame)->color);
                color_haut = clair_couleur(((ei_frame_t *)frame)->color);
                break;
            case ei_relief_sunken:
                color_bas = clair_couleur(((ei_frame_t *)frame)->color);
                color_haut = foncer_couleur(((ei_frame_t *)frame)->color);
                break;
            case ei_relief_none:
                color_bas = ((ei_frame_t *)frame)->color;
                color_haut = ((ei_frame_t *)frame)->color;
            default:
                break;
        }

        ei_linked_point_t *points_bas = dessine_rectangle(frame->screen_location, "bas");
        ei_draw_polygon(surface, points_bas, color_bas, clipper);
        ei_linked_point_t *points_haut = dessine_rectangle(frame->screen_location, "haut");
        ei_draw_polygon(surface, points_haut, color_haut, clipper);
        ei_linked_point_t *points = dessine_rectangle(*frame->content_rect, "all");
        ei_draw_polygon(surface, points, ((ei_frame_t*)frame)->color, clipper);
        ei_draw_polygon(pick_surface, points, *frame->pick_color, clipper);


        if (((ei_frame_t *)frame)->text)
        {
            ei_point_t where;
            ei_size_t size = frame->requested_size;
            if (((ei_frame_t *)frame)->text_anchor == ei_anc_center)
            {

                where = ei_point_add(frame->content_rect->top_left, ei_point(size.width/5,size.height*0.1));
            }
            ei_draw_text(surface, &where, ((ei_frame_t *)frame)->text, ((ei_frame_t *)frame)->text_font, ((ei_frame_t *)frame)->text_color, frame->content_rect);
        }

        if (((ei_frame_t *)frame)->img)
        {
            ei_copy_surface(surface, frame->content_rect , ((ei_frame_t *)frame)->img, ((ei_frame_t *)frame)->img_rect, EI_TRUE);
        }

    }
    else if (frame == ei_app_root_widget())
    {
        ei_linked_point_t *points = dessine_rectangle(frame->screen_location, "all");
        ei_draw_polygon(surface, points, ((ei_frame_t*)frame)->color, clipper);
        ei_draw_polygon(pick_surface, points, *(frame->pick_color), clipper);
    }

}
ei_linked_point_t *dessine_rectangle(ei_rect_t rect, char *precision)
{
    ei_linked_point_t *points_haut_droite = malloc(sizeof (ei_linked_point_t));
    ei_linked_point_t *points_bas_droite = malloc(sizeof (ei_linked_point_t));
    ei_linked_point_t *points_haut_gauche = malloc(sizeof (ei_linked_point_t));
    ei_linked_point_t *points_bas_gauche = malloc(sizeof (ei_linked_point_t));

    points_haut_droite->point = ei_point_add(rect.top_left, ei_point(rect.size.width, 0));
    points_haut_droite->next = NULL;
    points_haut_gauche->point = rect.top_left;
    points_haut_gauche->next = NULL;
    points_bas_gauche->point = ei_point_add(rect.top_left, ei_point(0, rect.size.height));
    points_bas_gauche->next = NULL;
    points_bas_droite->point = ei_point_add(rect.top_left, ei_point(rect.size.width, rect.size.height));
    points_bas_droite->next = NULL;

    if (strcmp(precision, "all") == 0)
    {
        points_haut_droite->next  = points_haut_gauche;
        points_haut_gauche->next = points_bas_gauche;
        points_bas_gauche->next = points_bas_droite;
    }
    else if (strcmp(precision, "haut") == 0)
    {
        points_haut_droite->next  = points_haut_gauche;
        points_haut_gauche->next = points_bas_gauche;
    }
    else if (strcmp(precision, "bas") == 0)
    {
        points_haut_droite->next  = points_bas_gauche;
        points_bas_gauche->next = points_bas_droite;
    }
    return points_haut_droite;



}
void frame_geomnotifyfunc_t(struct ei_widget_t* widget){}

void frame_setdefaultsfunc_t	(struct ei_widget_t *frame)
{
    ((ei_frame_t*)frame)->color        = ei_default_background_color ;
    ((ei_frame_t*)frame)->relief       = ei_relief_none;
    ((ei_frame_t*)frame)->text         = NULL;
    ((ei_frame_t*)frame)->img          = NULL;
    ((ei_frame_t*)frame)->text_font    = ei_default_font;
    ((ei_frame_t*)frame)->text_color   = ei_font_default_color;
    ((ei_frame_t*)frame)->text_anchor  = ei_anc_center;
    ((ei_frame_t*)frame)->img_rect     = NULL;
    ((ei_frame_t*)frame)->img_anchor   = ei_anc_center ;
    ((ei_frame_t*)frame)->border_width = 0;

    if (frame != ei_app_root_widget())
        {
            frame->requested_size = ei_size_zero();
            frame->screen_location.top_left = frame->parent->content_rect->top_left;
            frame->content_rect = malloc(sizeof (ei_rect_t));
            *(frame->content_rect)= frame->screen_location;
        }
}

void draw_hierarchie(ei_widget_t *widget, ei_surface_t offscreen, ei_rect_t *rect)
{
    // draws all the hierarchy of the widgets ( recursively )
    if (widget)
    {
        if (rect == NULL)
        {
            widget->wclass->drawfunc(widget, ei_app_root_surface(), offscreen, parent_screen_location(widget));
        }
        else
        {

            widget->wclass->drawfunc(widget, ei_app_root_surface(), offscreen, rect);
        }
        ei_widget_t *tmp_fils = widget->children_head;
        while (tmp_fils != NULL)
        {
            draw_hierarchie(tmp_fils, offscreen, rect);
            tmp_fils = tmp_fils->next_sibling;
        }
    }



}
ei_widget_t *recherche_id(ei_widget_t *widget, uint32_t pick_id)
{
    if (widget->pick_id == pick_id)
    {
        return widget;
    }

}

ei_rect_t *parent_screen_location(ei_widget_t *widget)
{
    if (widget == ei_app_root_widget())
    {
        return NULL;
    }
    return &widget->parent->screen_location;
}

void ajoute_fils(ei_widget_t *widget_parent, ei_widget_t *widgetFils)
{
    if (widget_parent->children_head == NULL)
    {
        widget_parent->children_head = widgetFils;
        //widget_parent->next_sibling  = widgetFils;
    }
    else
    {
        widget_parent->children_tail->next_sibling = widgetFils;
    }
    widget_parent->children_tail = widgetFils;
}

ei_color_t *couleur(uint32_t id)
{
    ei_color_t *eiColor = malloc(sizeof(ei_color_t));
    eiColor->red = (unsigned char) id ;
    eiColor->blue = (unsigned char) id + 1;
    eiColor->green = (unsigned char) id + 2;
    eiColor->alpha = 0xff;
}

ei_rect_t *ei_rect_widget(ei_widget_t *widget)
{
    return &widget->screen_location;
}

ei_bool_t quit(ei_widget_t* widget, ei_event_t* event, void* user_param)
{
    //ei_app_quit_request();
    ei_widget_destroy(widget->parent);
    hw_surface_lock(ei_app_root_surface());
    hw_surface_lock(ei_app_offscreen());
    draw_hierarchie(ei_app_root_widget(), ei_app_offscreen(), NULL);
    hw_surface_unlock(ei_app_root_surface());
    hw_surface_unlock(ei_app_offscreen());
    hw_surface_update_rects(ei_app_root_surface(), NULL);
}

ei_bool_t debut_deplacement(ei_widget_t *widget, struct ei_event_t *event, void *user_param)
{
    //printf("%s", ei_widget_pick(&event->param.mouse.where)->wclass->name);
    int hauteur_ruban = hw_surface_get_size(hw_text_create_surface(((ei_toplevel_t*)widget)->title, ei_default_font, ei_font_default_color)).height;

    if (event->type == ei_ev_mouse_buttondown && widget->screen_location.top_left.x<=event->param.mouse.where.x
    && event->param.mouse.where.x<=widget->screen_location.top_left.x+widget->requested_size.width+((ei_toplevel_t*)widget)->border_width &&
    widget->screen_location.top_left.y<=event->param.mouse.where.y
    && event->param.mouse.where.y<=widget->screen_location.top_left.y+hauteur_ruban)
    {
        ei_bind(ei_ev_mouse_move, widget, NULL, deplacement_move, NULL);
        ei_bind(ei_ev_mouse_buttonup, widget, NULL, fin_deplacement, NULL);
    }
    return EI_FALSE;
}

ei_bool_t fin_deplacement(ei_widget_t *widget, struct ei_event_t *event, void *user_param)
{
    ei_unbind(ei_ev_mouse_move, widget, NULL, deplacement_move, NULL);
    ei_unbind(ei_ev_mouse_buttonup, widget, NULL, fin_deplacement, NULL);

    return EI_FALSE;

}

ei_bool_t deplacement_move(ei_widget_t *widget, struct ei_event_t *event, void *user_param)
{
    ei_point_t where_to = event->param.mouse.where;
    //hw_event_schedule_app(166, NULL);
    // 1 . on demande de rafraichir la surface contenant le toplevel
    // 2 . on deplace le toplevel et toute sa descendance
    // 2.0 . on l'enleve de son ancienne position (si nécessaire)
    // 2.1 . on deplace d'abord le toplevel
    // 2.2 . on deplace sa descendance relativement à lui
    // 3. on demande de raffraichir la nouvelle surface contenant le toplevel
    ei_app_invalidate_rect(&widget->screen_location); // pour rafraichir le rectangle
    int hauteur_ruban = hw_surface_get_size(hw_text_create_surface(((ei_toplevel_t*)widget)->title, ei_default_font, ei_font_default_color)).height;
    int axe_x = where_to.x - where_from_func().x; // axe_x : le deplacement /x
    int axe_y = where_to.y - where_from_func().y;
    int constante_x = widget->screen_location.top_left.x + axe_x;
    int constante_y = widget->screen_location.top_left.y + axe_y;
    ei_place(widget,NULL, &constante_x, &constante_y, NULL,NULL,NULL,NULL,NULL,NULL);
    ei_app_invalidate_rect(&widget->screen_location);
    return EI_FALSE;

}

ei_bool_t debut_redimensionnement(ei_widget_t *widget, struct ei_event_t *event, void *user_param)
{
    int hauteur_ruban = hw_surface_get_size(hw_text_create_surface(((ei_toplevel_t*)widget)->title, ei_default_font, ei_font_default_color)).height;
    if (event->type == ei_ev_mouse_buttondown && widget->screen_location.top_left.x+widget->requested_size.width+2*((ei_toplevel_t*)widget)->border_width-hauteur_ruban/2<=event->param.mouse.where.x
        && event->param.mouse.where.x<=widget->screen_location.top_left.x+widget->requested_size.width+hauteur_ruban/2 &&
        widget->screen_location.top_left.y+hauteur_ruban+widget->requested_size.height-hauteur_ruban/2<=event->param.mouse.where.y
        && event->param.mouse.where.y<=widget->screen_location.top_left.y+hauteur_ruban+widget->requested_size.height+hauteur_ruban/2)
    {
        ei_bind(ei_ev_mouse_move, widget, NULL, redimensionnement_move, NULL);
        ei_bind(ei_ev_mouse_buttonup, widget, NULL, fin_redimensionnement, NULL);
    }
    return EI_FALSE;
}

ei_bool_t fin_redimensionnement(ei_widget_t *widget, struct ei_event_t *event, void *user_param)
{
    ei_unbind(ei_ev_mouse_move, widget, NULL, redimensionnement_move, NULL);
    ei_unbind(ei_ev_mouse_buttonup, widget, NULL, fin_redimensionnement, NULL);

    return EI_FALSE;
}


ei_bool_t redimensionnement_move(ei_widget_t *widget, struct ei_event_t *event, void *user_param)
{
    ei_point_t where_to = event->param.mouse.where;
    int hauteur_ruban = hw_surface_get_size(hw_text_create_surface(((ei_toplevel_t*)widget)->title, ei_default_font, ei_font_default_color)).height;
    int axe_x = where_to.x - where_from_func().x;
    int axe_y = where_to.y - where_from_func().y;
    int constante_x = hw_surface_get_size(hw_text_create_surface(((ei_toplevel_t*)widget)->title, ei_default_font, ei_font_default_color)).width;

    int width = (int) (widget->requested_size.width + axe_x);
    int height = (int) (widget->requested_size.height + axe_y);
    if (where_to.y >= widget->content_rect->top_left.y) // si on depasse pas la barre laterale
    {
        if (widget->content_rect->top_left.x + 2 * constante_x > where_to.x) {
            int new_width = where_to.x - widget->content_rect->top_left.x;
            int new_height = where_to.y - widget->content_rect->top_left.y;

            widget->requested_size.height = new_height;
            ei_place(widget,NULL, NULL,NULL , &widget->requested_size.width, &new_height,NULL,NULL,NULL,NULL);
            ei_app_invalidate_rect(&widget->screen_location);
        } else {
            int new_width = where_to.x - widget->content_rect->top_left.x;
            int new_height = where_to.y - widget->content_rect->top_left.y;

            widget->requested_size.height = new_height;
            widget->requested_size.width = new_width;
            ei_place(widget,NULL, NULL,NULL , &new_width, &new_height,NULL,NULL,NULL,NULL);
            ei_app_invalidate_rect(&widget->screen_location);
        }
    }
    return EI_FALSE;
}

//removes from screen
void delete_from_screen(ei_widget_t *widget)
{
    if (widget)
    {
        if (strcmp(widget->wclass->name, "toplevel") == 0)
        {
            ((ei_toplevel_t*)widget)->color.alpha = 0;
        }
        else if (strcmp(widget->wclass->name, "button") == 0)
        {
            ((ei_button_t *)widget)->color.alpha = 0;
        }
        else if (strcmp(widget->wclass->name, "frame") == 0)
        {
            ((ei_frame_t *)widget)->color.alpha = 0;
        }
        ei_widget_t *tmp_fils = widget->children_head;
        while (tmp_fils != NULL)
        {
            delete_from_screen(tmp_fils);
            tmp_fils = tmp_fils->next_sibling;
        }
    }
}

// returns the concerned widget in widget_pick function
ei_widget_t *widget_picked(ei_widget_t *widget, uint8_t red, uint8_t blue, uint8_t green)
{
    if (widget->pick_color->red == red && widget->pick_color->blue == blue && widget->pick_color->green == green)
    {
        return widget;
    }

    ei_widget_t *tmp_fils = widget->children_head;
    while (tmp_fils) {
        if (widget_picked(tmp_fils, red, blue, green))
        {
            return widget_picked(tmp_fils, red, blue, green);
        }
        else
        {
            tmp_fils = tmp_fils->next_sibling;
        }
    }
}

int size_of(ei_linked_rect_t *rect_list)
{
    ei_linked_rect_t *tmp = rect_list;
    int cpt = 0;
    while (tmp)
    {
        cpt ++;
        tmp = tmp->next;
    }
    return cpt;
}

//parameter widget is passed to children tail of his parent;
void changement_chainage(ei_widget_t *widget) {
    if (widget->parent->children_head == widget) {
        widget->parent->children_head = widget->parent->children_head->next_sibling;
    }
    else if (widget->parent->children_tail == widget)
    {

    }
    else {
        ei_widget_t *parcours = widget->parent->children_head;
        while (parcours->next_sibling != NULL && parcours != widget) {
            parcours = parcours->next_sibling;
        }
        if (parcours->next_sibling != NULL) {
            widget->parent->children_head = widget->parent->children_head->next_sibling;
        }
    }
    widget->parent->children_tail->next_sibling = widget;
    widget->parent->children_tail = widget;
}

// return the intersections of two_rects
ei_rect_t intersection(ei_rect_t premier_rect , ei_rect_t deux_rect){
    ei_rect_t rect_res = ei_rect_zero();
    int gauche = fmax(premier_rect.top_left.x, deux_rect.top_left.x);
    int droit = fmin(premier_rect.top_left.x + premier_rect.size.width, deux_rect.top_left.x + deux_rect.size.width);
    int bas = fmin(premier_rect.top_left.y + premier_rect.size.height , deux_rect.top_left.y+ deux_rect.size.height);
    int haut = fmax(premier_rect.top_left.y, deux_rect.top_left.y);
    if (droit > gauche) {
        if (bas>haut) {
            rect_res.top_left = ei_point(gauche,haut);
            rect_res.size.height = bas - haut;
            rect_res.size.width = droit - gauche;
        }
    }
    return rect_res;
}
