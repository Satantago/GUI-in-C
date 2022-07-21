#include "ei_widget.h"
#include "stdlib.h"
#include "ei_utils.h"
#include "math.h"
#include "ei_button.h"
#include "ei_application.h"
#include "ei_draw.h"
#include "stdio.h"
#include "ei_event.h"
#include "ei_application_private.h"
void free_linked(ei_linked_point_t *head_struct)
{
    struct ei_linked_point_t *current = head_struct;
    struct ei_linked_point_t *suivant;
    while (current->next) {
        suivant = current->next;
        free(current);
        current = suivant;
    }
    free(current);
}


struct ei_widget_t* button_allocfunc_t(void)
{
    ei_button_t *button = calloc(1, sizeof(ei_button_t));
    return (ei_widget_t*)button;
}
void button_releasefunc_t(struct ei_widget_t * button)
{
    free(button);
}

void    button_drawfunc_t	(struct ei_widget_t *	widget,
                              ei_surface_t		surface,
                              ei_surface_t		pick_surface,
                              ei_rect_t*		clipper)
{
    if ((widget->geom_params != NULL && widget != ei_app_root_widget()) || widget == ei_app_root_widget())
    {
        ei_color_t color_bas;
        ei_color_t color_haut;

        switch (((ei_button_t*)widget)->relief)
        {
            case  ei_relief_raised:
                color_bas = foncer_couleur(((ei_button_t*)widget)->color);
                color_haut = clair_couleur(((ei_button_t*)widget)->color);
                break;
            case ei_relief_sunken:
                color_bas = clair_couleur(((ei_button_t*)widget)->color);
                color_haut = foncer_couleur(((ei_button_t*)widget)->color);
                break;
            case ei_relief_none:
                color_bas = ((ei_button_t*)widget)->color;
                color_haut = ((ei_button_t*)widget)->color;
            default:
                break;
        }

        ei_linked_point_t *points_haut = rounded_frame(widget->screen_location, ((ei_button_t*)widget)->corner_radius, "haut");
        ei_linked_point_t *points_bas  = rounded_frame(widget->screen_location, ((ei_button_t*)widget)->corner_radius, "bas") ;
        ei_linked_point_t *points      = rounded_frame( *widget->content_rect,((ei_button_t*)widget)->corner_radius, "all")   ;

        ei_draw_polygon(surface, points_haut, color_haut, clipper)             ;
        ei_draw_polygon(surface, points_bas, color_bas, clipper)               ;
        ei_draw_polygon(surface, points,((ei_button_t*)widget)->color, clipper);

        ei_draw_polygon(pick_surface, points_haut, *widget->pick_color, clipper);
        ei_draw_polygon(pick_surface, points_bas,*widget->pick_color , clipper) ;
        ei_draw_polygon(pick_surface, points,*widget->pick_color, clipper)      ;

        if (((ei_button_t*)widget)->text)
        {
            ei_point_t where;
            ei_size_t size = widget->requested_size;

            if (((ei_button_t*)widget)->text_anchor == ei_anc_center)
            {

                where = ei_point_add(widget->content_rect->top_left, ei_point(size.width/5,size.height*0.4));
            }
            //ei_rect_t clipper = ei_rect(ei_point_zero(), ei_size_scale(widget->content_rect->size, 0.7));
            ei_draw_text(surface, &where, ((ei_button_t*)widget)->text, ((ei_button_t*)widget)->text_font, ((ei_button_t*)widget)->text_color, widget->content_rect);
        }

        if (((ei_button_t*)widget)->img)
        {
            ei_copy_surface(surface, widget->content_rect , ((ei_button_t*)widget)->img, ((ei_button_t*)widget)->img_rect, EI_TRUE);
        }
        free_linked(points_haut);
        free_linked(points_bas) ;
        free_linked(points)     ;
    }

}
void button_geomnotifyfunc_t(struct ei_widget_t* button){}

void button_setdefaultsfunc_t	(struct ei_widget_t *button)
{
    ((ei_button_t*)button)->relief        = ei_relief_raised                      ;
    ((ei_button_t*)button)->border_width  = k_default_button_border_width         ;
    ((ei_button_t*)button)->corner_radius = k_default_button_corner_radius        ;
    ((ei_button_t*)button)->callback      = NULL                                  ;
    ((ei_button_t*)button)->text_font     = ei_default_font                       ;
    ((ei_button_t *)button)->color        = ei_default_background_color           ;
    ((ei_button_t *)button)->text         = NULL                                  ;
    ((ei_button_t *)button)->img          = NULL                                  ;
    ((ei_button_t *)button)->text_color   = ei_font_default_color                 ;
    ((ei_button_t *)button)->text_anchor  = ei_anc_center                         ;
    ((ei_button_t *)button)->img_rect     = malloc(sizeof(ei_rect_t))        ;
    ((ei_button_t *)button)->img_anchor   = ei_anc_center                         ;
    button->requested_size                = ei_size_zero()                        ;
    button->screen_location.top_left      = button->parent->content_rect->top_left;
    button->content_rect                  = malloc(sizeof (ei_rect_t))       ;
    *(button->content_rect)               = button->screen_location               ;
}

// returns the x used in arc function
int abscisse(int x_centre, int radius, double angle)
{
    return (int) x_centre + radius * cos(angle);
}
// return the y used in arc function
int ordonnee(int y_centre, int radius, double angle)
{
    return (int) y_centre - radius * sin(angle);

}

//draws an arc (part of a cercle) beetwen two angles given as parameters

ei_linked_point_t *arc(ei_point_t centre, int rayon, double angle_debut, double angle_fin){

    int k;
    int n =10;
    ei_linked_point_t *points = malloc(sizeof (ei_linked_point_t));
    points->point.x = abscisse(centre.x, rayon, angle_debut);
    points->point.y = ordonnee(centre.y, rayon, angle_debut);
    points->next = NULL;
    for (k = 1; k<=n; k++)
    {
        inserer_queue(&points,  abscisse(centre.x, rayon, angle_debut + k*(angle_fin - angle_debut)/n),
                      ordonnee(centre.y, rayon, angle_debut + k*(angle_fin - angle_debut)/n));
    }
    return points;
}

// print les pixels d'une surface
void print_surface(ei_surface_t surface)
{
    printf("On commence l'affichage\n");
    uint32_t*			pixel;
    pixel = (uint32_t *)hw_surface_get_buffer(surface);
    for (int i = 0; i< hw_surface_get_size(surface).height* hw_surface_get_size(surface).width; i++)
    {

        printf("| %u | ",pixel[i]);

    }
}

// returns a pointer to the last point of the ei_linked_point_t structure given
ei_linked_point_t *queue(ei_linked_point_t *points)
{
    ei_linked_point_t *tmp = points;
    while (tmp->next != NULL)
    {
        tmp = tmp->next;
    }
    return tmp;
}

// adds a point to the given ei_linked_point_t structure
void inserer_queue(struct ei_linked_point_t **pl, int x, int y)
{

    struct ei_linked_point_t cell   = {-1,-1,*pl};
    struct ei_linked_point_t *queue = &cell                          ;

    while (queue->next !=NULL)
    {
        queue = queue->next;
    }
    queue->next          = malloc(sizeof(struct ei_linked_point_t));
    queue->next->point.x = x                                            ;
    queue->next->point.y = y                                            ;
    queue->next->next    = NULL                                         ;
    *pl                  = cell.next                                    ;
}

// returns an ei_linked_point_structure with circules edges of the origin rectangle
// it used to draw a button

ei_linked_point_t *rounded_frame(ei_rect_t rectangle, int rayon_arrondi, char *precision)
{
    ei_point_t centre               = ei_point_sub(rectangle.top_left, ei_point(-rayon_arrondi, -rayon_arrondi)) ;

    double angle_debut_haut_gauche  = M_PI/2                                                                                   ;
    double angle_fin_haut_gauche    = M_PI                                                                                     ;

    double angle_debut_haut_droite  = M_PI/4                                                                                   ;
    double angle_fin_haut_droite    = M_PI/2                                                                                   ;

    double angle_debut_bas_gauche  = M_PI                                                                                      ;
    double angle_fin_bas_gauche    = 5*M_PI/4                                                                                  ;

    double angle_debut_bas_droite  = 3*M_PI/2                                                                                  ;
    double angle_fin_bas_droite    = 2*M_PI                                                                                    ;

    ei_point_t centre_haut_droite = {centre.x + rectangle.size.width-2*rayon_arrondi      ,             centre.y                            };
    ei_point_t centre_haut_gauche = {      centre.x                                       ,             centre.y                            };
    ei_point_t centre_bas_gauche  = {      centre.x                                       , centre.y + rectangle.size.height-2*rayon_arrondi};
    ei_point_t centre_bas_droite  = {      centre.x + rectangle.size.width-2*rayon_arrondi, centre.y + rectangle.size.height-2*rayon_arrondi};

    ei_linked_point_t *points_haut_droite     = arc(centre_haut_droite , rayon_arrondi
                                                    , angle_debut_haut_droite, angle_fin_haut_droite);
    ei_linked_point_t *points_haut_gauche     = arc(centre_haut_gauche , rayon_arrondi,
                                                    angle_debut_haut_gauche, angle_fin_haut_gauche);
    ei_linked_point_t *points_bas_droite      = arc(centre_bas_droite  , rayon_arrondi,
                                                    angle_debut_bas_droite, angle_fin_bas_droite);
    ei_linked_point_t *points_bas_gauche      = arc(centre_bas_gauche  , rayon_arrondi,
                                                    angle_debut_bas_gauche, angle_fin_bas_gauche);
    ei_linked_point_t *points_bas_gauche_bas  = arc(centre_bas_gauche  , rayon_arrondi,
                                                    5*M_PI/4, 3*M_PI/2);
    ei_linked_point_t *points_haut_droite_bas = arc(centre_haut_droite , rayon_arrondi,
                                                    0, M_PI/4);

    // This is used for the relief

    if (strcmp(precision, "all") == 0)
    {
        queue(points_haut_droite)->next = points_haut_gauche;
        queue(points_haut_gauche)->next = points_bas_gauche;
        queue(points_bas_gauche)->next = points_bas_gauche_bas;
        queue(points_bas_gauche_bas)->next = points_bas_droite;
        queue(points_bas_droite)->next = points_haut_droite_bas;
        return points_haut_droite;
    }

    else if (strcmp(precision, "haut") == 0)
    {
        queue(points_haut_droite)->next = points_haut_gauche;
        queue(points_haut_gauche)->next = points_bas_gauche;
        return points_haut_droite;
    }

    else if (strcmp(precision, "bas") == 0)
    {
        queue(points_haut_droite_bas)->next = points_bas_gauche_bas;
        queue(points_bas_gauche_bas)->next = points_bas_droite;
        return points_haut_droite_bas;
    }
}


//returns a darker color
ei_color_t foncer_couleur(ei_color_t color)
{
    ei_color_t eiColor;
    eiColor.red   = color.red * 0.75  ;
    eiColor.alpha = color.alpha       ;
    eiColor.green = color.green * 0.75;
    eiColor.blue  = color.blue * 0.75 ;
    return eiColor;
}

//returns a lighter color
ei_color_t clair_couleur(ei_color_t color)
{
    ei_color_t eiColor;
    eiColor.red   = color.red * 1.25  ;
    eiColor.alpha = color.alpha       ;
    eiColor.green = color.green * 1.25;
    eiColor.blue  = color.blue * 1.25 ;
    return eiColor;
}

// This function is called after every mousedown of the structure widget button
ei_bool_t bouton_enfonce(ei_widget_t *widget, struct ei_event_t *event, void *user_param)
{
    if (event->type == ei_ev_mouse_buttondown && widget->screen_location.top_left.x<=event->param.mouse.where.x
    && event->param.mouse.where.x<=widget->screen_location.top_left.x+widget->requested_size.width &&
    widget->screen_location.top_left.y<=event->param.mouse.where.y
    && event->param.mouse.where.y<=widget->screen_location.top_left.y+widget->requested_size.height)
    {
        ((ei_button_t*)widget)->relief = ei_relief_sunken;
        ei_app_invalidate_rect(&widget->screen_location);
        ei_bind(ei_ev_mouse_buttonup, widget, NULL, bouton_relache, NULL);
        ei_bind(ei_ev_mouse_move, widget, NULL, bouton_relache, NULL);
        if (((ei_button_t*)widget)->callback)
        {
            return ((ei_button_t*)widget)->callback(widget, event, widget->user_data);
        }
    }
    return EI_FALSE;

}
//this function is called after every mouse_up concerning widget button
ei_bool_t bouton_relache(ei_widget_t *widget, struct ei_event_t *event, void *user_param)
{
    if (event->type == ei_ev_mouse_buttonup || !(widget->screen_location.top_left.x<=event->param.mouse.where.x
    && event->param.mouse.where.x<=widget->screen_location.top_left.x+widget->requested_size.width &&
    widget->screen_location.top_left.y<=event->param.mouse.where.y
    && event->param.mouse.where.y<=widget->screen_location.top_left.y+widget->requested_size.height))
    {
        ((ei_button_t*)widget)->relief = ei_relief_raised;
        ei_app_invalidate_rect(&widget->screen_location);
        ei_unbind(ei_ev_mouse_buttonup, widget, NULL, bouton_relache, NULL);
        ei_unbind(ei_ev_mouse_move, widget, NULL, bouton_relache, NULL);
    }
    return EI_FALSE;
}