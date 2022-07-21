#include "ei_widget.h"
#include "ei_widgetclass.h"
#include "ei_frame.h"
#include "assert.h"
#include "ei_button.h"
#include "ei_application.h"
#include "ei_toplevel.h"
#include "ei_utils.h"
uint32_t id = 0;
ei_widget_t*		ei_widget_create		(ei_widgetclass_name_t	class_name,
                                             ei_widget_t*		parent,
                                             void*			user_data,
                                             ei_widget_destructor_t destructor){
    // verifie que la classe dont le nom est passée en param est connue par la bibliotheque ..
    ei_widgetclass_t*	widget_to_create = ei_widgetclass_from_name(class_name);
    // verifie qu'il l'a trouvé ..
    assert(widget_to_create!=NULL);
    // appelle la fonction d'allocation de widget de la classe
    ei_widget_t *new_widget = widget_to_create->allocfunc();
    // initialise les attributs communs à tous les widgets ( classe, parent, descendante, etc )
    new_widget->wclass = widget_to_create;
    new_widget->user_data = user_data;
    new_widget->destructor = destructor;
    new_widget->parent = parent;
    new_widget->pick_id = id + 1;
    id ++;
    new_widget->pick_color = couleur(new_widget->pick_id);
    ajoute_fils(parent, new_widget);
    //  By defaults, points to the screen_location.
    //new_widget->content_rect = &new_widget->screen_location;
    //parent->next_sibling = new_widget;
    // appelle la fonction d'initialisation des attributs spécifiques à la classe
    widget_to_create->setdefaultsfunc(new_widget);
    return new_widget;
}

void			ei_frame_configure		(ei_widget_t*		widget,
                                           ei_size_t*		requested_size,
                                           const ei_color_t*	color,
                                           int*			border_width,
                                           ei_relief_t*		relief,
                                           char**			text,
                                           ei_font_t*		text_font,
                                           ei_color_t*		text_color,
                                           ei_anchor_t*		text_anchor,
                                           ei_surface_t*		img,
                                           ei_rect_t**		img_rect,
                                           ei_anchor_t*		img_anchor)
{
    if (color!=NULL){((ei_frame_t *)widget)->color = *color;}
    if (border_width!=NULL){((ei_frame_t *)widget)->border_width = *border_width;}
    if (requested_size!=NULL){widget->requested_size = *requested_size;}
    if (relief!=NULL){((ei_frame_t *)widget)->relief = *relief;}
    if (text_anchor!=NULL){((ei_frame_t *)widget)->text_anchor = *text_anchor;}
    if (text)
    {
        if (*text)
        {
            if (((ei_frame_t *)widget)->text == NULL)       // si on a pas encore definit de texte pour un frame donné
            {
                ((ei_frame_t *)widget)->text = malloc(sizeof(char)); // malloc(strlen(texte)*sizeof(char)
                strcpy(((ei_frame_t *)widget)->text, *text)  ;
            }
            else
            {
                strcpy(((ei_frame_t *)widget)->text, *text)  ;
            }
        }
        else
        {
            ((ei_frame_t *)widget)->text = NULL;
        }


    }

    if (text_font!=NULL){((ei_frame_t *)widget)->text_font = *text_font;}
    if (text_color!=NULL){((ei_frame_t *)widget)->text_color = *text_color;}
    if (img_rect!=NULL){((ei_frame_t *)widget)->img_rect = *img_rect;}
    if (img!=NULL){((ei_frame_t *)widget)->img = *img;}
    if (img_anchor!=NULL){((ei_frame_t *)widget)->img_anchor = *img_anchor;}

    if (widget != ei_app_root_widget())
    {

        widget->screen_location.size = ei_size(widget->requested_size.width+2*((ei_frame_t *)widget)->border_width,
                                               widget->requested_size.height+2*((ei_frame_t *)widget)->border_width);
        widget->content_rect->size = widget->requested_size;
        ei_point_t point_border = ei_point_add(widget->screen_location.top_left, ei_point(((ei_frame_t *)widget)->border_width, ((ei_frame_t *)widget)->border_width)) ;
        widget->content_rect->top_left = point_border;
    }
}

void			ei_button_configure		(ei_widget_t*		widget,
                                            ei_size_t*		requested_size,
                                            const ei_color_t*	color,
                                            int*			border_width,
                                            int*			corner_radius,
                                            ei_relief_t*		relief,
                                            char**			text,
                                            ei_font_t*		text_font,
                                            ei_color_t*		text_color,
                                            ei_anchor_t*		text_anchor,
                                            ei_surface_t*		img,
                                            ei_rect_t**		img_rect,
                                            ei_anchor_t*		img_anchor,
                                            ei_callback_t*		callback,
                                            void**			user_param)
{

    if (color!=NULL){((ei_button_t*)widget)->color = *color;}
    if (border_width!=NULL){((ei_button_t*)widget)->border_width = *border_width;}
    if (requested_size!=NULL){widget->requested_size = *requested_size;}
    if (corner_radius!=NULL){((ei_button_t*)widget)->corner_radius = *corner_radius;}
    if (relief!=NULL){((ei_button_t*)widget)->relief = *relief;}
    if (text_anchor!=NULL){((ei_button_t*)widget)->text_anchor = *text_anchor;}
    if (text!=NULL){((ei_button_t*)widget)->text = malloc(21);
        strcpy(((ei_button_t *)widget)->text, *text)  ; }
    if (text_font!=NULL){((ei_button_t*)widget)->text_font = *text_font;}
    if (text_color!=NULL){((ei_button_t*)widget)->text_color = *text_color;}
    if (img_rect!=NULL){*(((ei_button_t*)widget)->img_rect) = **img_rect;}
    if (img!=NULL){
        ((ei_button_t*)widget)->img = hw_surface_create(ei_app_root_surface(), hw_surface_get_size(*img), EI_TRUE);
        ei_copy_surface(((ei_button_t*)widget)->img, NULL,*img, NULL, EI_TRUE );}
    if (img_anchor!=NULL){((ei_button_t*)widget)->img_anchor = *img_anchor;}
    if (callback!=NULL){((ei_button_t*)widget)->callback = *callback;}
    if (user_param!=NULL){widget->user_data = *user_param;}
    if (requested_size ==NULL){widget->requested_size = widget->parent->content_rect->size;}
    widget->screen_location.size = ei_size(widget->requested_size.width+2*((ei_button_t *)widget)->border_width,
                                           widget->requested_size.height+2*((ei_button_t *)widget)->border_width);
    widget->screen_location.top_left = widget->parent->content_rect->top_left;
    ei_point_t point_border = ei_point_add(widget->screen_location.top_left, ei_point(((ei_button_t *)widget)->border_width, ((ei_button_t *)widget)->border_width));
    widget->content_rect = malloc(sizeof (ei_rect_t));
    widget->content_rect->size = widget->requested_size;
    widget->content_rect->top_left = point_border;
}
void			ei_toplevel_configure		(ei_widget_t*		widget,
                                              ei_size_t*		requested_size,
                                              ei_color_t*		color,
                                              int*			border_width,
                                              char**			title,
                                              ei_bool_t*		closable,
                                              ei_axis_set_t*		resizable,
                                              ei_size_t**		min_size)
{

    if (color!=NULL){((ei_button_t*)widget)->color = *color;}
    if (border_width!=NULL){((ei_toplevel_t *)widget)->border_width = *border_width;}
    if (requested_size!=NULL){widget->requested_size = *requested_size;}
    if (title!=NULL){((ei_toplevel_t *)widget)->title = malloc(21);
        strcpy(((ei_toplevel_t *)widget)->title, *title);}
    if (closable!=NULL){((ei_toplevel_t *)widget)->closable = *closable;}
    if (resizable!=NULL){((ei_toplevel_t *)widget)->resizable = *resizable;}
    if (min_size!=NULL){((ei_toplevel_t *)widget)->min_size = *min_size;}
    if (requested_size ==NULL){widget->requested_size = widget->parent->content_rect->size;}
    int hauteur_ruban = hw_surface_get_size(hw_text_create_surface(((ei_toplevel_t*)widget)->title, ei_default_font, ei_font_default_color)).height;
    widget->screen_location.top_left = widget->parent->content_rect->top_left;

    widget->screen_location.size = ei_size(widget->requested_size.width + 2*((ei_toplevel_t*)widget)->border_width,
                                           widget->requested_size.height + ((ei_toplevel_t*)widget)->border_width + hauteur_ruban);
    widget->content_rect = malloc(sizeof (ei_rect_t));
    widget->content_rect->size = widget->requested_size;
    widget->content_rect->top_left = ei_point(((ei_toplevel_t*)widget)->border_width , hauteur_ruban);
}


void			ei_widget_destroy		(ei_widget_t*		widget)
{/*
    if (widget->geom_params) // ie the widget is currently displayed
    {
         //widget->geom_params->manager->releasefunc();                         // removes the widget from the screen
    }
    ei_widget_t *child = widget->children_head;
    while (child != NULL)
    {
        ei_widget_destroy(child); // Destroys all its descendants
        child = child->next_sibling;
    }
    if (widget->destructor)
    {
        widget->destructor(widget); // Calls its destructor if it was provided
    }
    widget->wclass->releasefunc(widget); // Frees the memory used by the widget (e.g. the widget param)
*/
    ei_widget_t *tmp_fils = widget->parent->children_head;
    if (tmp_fils == widget)
    {
        widget->parent->children_head = NULL;
        widget->parent->children_tail = NULL;

    }
    else
    {
        while (tmp_fils)
        {
            if (tmp_fils->next_sibling == widget)
            {
                if ( tmp_fils->next_sibling)
                {
                    tmp_fils->next_sibling = tmp_fils->next_sibling->next_sibling;
                }
                else
                {
                    tmp_fils->next_sibling = NULL;
                }
            }
            tmp_fils = tmp_fils->next_sibling;
        }
    }

    ei_app_invalidate_rect(&widget->screen_location);
}


/**
 * @brief	Returns the widget that is at a given location on screen.
 *
 * @param	where		The location on screen, expressed in the root window coordinates.
 *
 * @return			The top-most widget at this location, or NULL if there is no widget
 *				at this location (except for the root widget).
 */
ei_widget_t*		ei_widget_pick			(ei_point_t*		where)
{
    /*
    uint32_t *ptr;
    //ptr = (uint32_t *)hw_surface_get_buffer(OFFSCREEN);
    uint32_t  ID = ptr[where->x + where->y];
    return recherche_id(ei_app_root_widget(), ID); // search for the widget whom id is ID
     */
    int ir ;
    int ig ;
    int ib ;
    int ia ;
    hw_surface_get_channel_indices(ei_app_offscreen(), &ir, &ig, &ib, &ia);
    //uint32_t *buffer = (uint32_t*) hw_surface_get_buffer(ei_app_offscreen());
    uint32_t *ptr_octet_wanted = (uint32_t *) (hw_surface_get_buffer(ei_app_offscreen())) + where->x +(where->y*hw_surface_get_size(ei_app_offscreen()).width);
    uint8_t red = ((uint8_t *)ptr_octet_wanted)[ir];
    uint8_t blue = ((uint8_t *)ptr_octet_wanted)[ib];
    uint8_t green = ((uint8_t *)ptr_octet_wanted)[ig];

    return widget_picked(ei_app_root_widget(), red, blue, green);
}