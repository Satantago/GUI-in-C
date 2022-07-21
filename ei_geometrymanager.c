#include "ei_geometrymanager.h"
#include "ei_placer.h"
#include "stdlib.h"
#include "ei_types.h"
#include "ei_frame.h"
#include "ei_toplevel.h"
#include "ei_application.h"
#include "ei_utils.h"
// placer is a geometry manager ..

typedef struct structure_placeur {
    ei_geometrymanager_t *manager;
    ei_anchor_t anchor;
    int x;
    int y;
    int width;
    int height;
    float rel_x;
    float rel_y;
    float rel_width;
    float rel_height;
} structure_placeur;

void	placer_runfunc_t	(struct ei_widget_t*	widget) {

    //configures size
    if (widget->content_rect == NULL)
    {
        if (strcmp(widget->wclass->name, "toplevel")==0)
        {
            ei_toplevel_configure		(widget,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL);
        }
        else if (strcmp(widget->wclass->name, "button")==0)
        {
            ei_button_configure		(widget,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL);
        }

        else if (strcmp(widget->wclass->name, "frame")==0)

        {
            ei_frame_configure		(widget,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL);
        }

    }
    int width = ((structure_placeur*) widget->geom_params)->width;
    int height = ((structure_placeur*) widget->geom_params)->height;
    float rel_width = ((structure_placeur*) widget->geom_params)->rel_width;
    float rel_height = ((structure_placeur*) widget->geom_params)->rel_height;
    ei_size_t parent_size = widget->parent->content_rect->size;
    ei_size_t parent_size1 = widget->parent->content_rect->size;
    int constante_x = widget->content_rect->top_left.x - widget->screen_location.top_left.x;
    int constante_y = widget->content_rect->top_left.y - widget->screen_location.top_left.y;

    widget->screen_location.size.width = (int) (width + 2*constante_x + parent_size.width * rel_width);
    widget->screen_location.size.height = (int) (height+ constante_y + constante_x + parent_size.height * rel_height);
    widget->content_rect->size.width = widget->screen_location.size.width - 2*constante_x;
    widget->content_rect->size.height = widget->screen_location.size.height -constante_y - constante_x;

    widget->requested_size = widget->content_rect->size;

    int old_topleft_x = widget->parent->content_rect->top_left.x;
    int old_topleft_y = widget->parent->content_rect->top_left.y;
    //configures top left
    ei_anchor_t anchor = ((structure_placeur*) widget->geom_params)->anchor;
    int x = ((structure_placeur*) widget->geom_params)->x;
    int y = ((structure_placeur*) widget->geom_params)->y;
    float rel_x = ((structure_placeur*) widget->geom_params)->rel_x;
    float rel_y = ((structure_placeur*) widget->geom_params)->rel_y;

    width = widget->screen_location.size.width;
    height = widget->screen_location.size.height;

    if (anchor == ei_anc_northwest) {
        widget->screen_location.top_left.x = (int) (old_topleft_x + (rel_x) * parent_size.width + x);
        widget->screen_location.top_left.y = (int) (old_topleft_y + (rel_y) * parent_size.height + y);
    }

    else if (anchor == ei_anc_northeast) {
        widget->screen_location.top_left.x = (int) (old_topleft_x  + (rel_x) * parent_size.width + x
                                                    - width);
        widget->screen_location.top_left.y = (int) (old_topleft_y + (rel_y) * parent_size.height + y);
    }

    else if (anchor == ei_anc_southwest) {
        widget->screen_location.top_left.x = (int) (old_topleft_x + (rel_x) * parent_size.width + x);
        widget->screen_location.top_left.y = (int) (old_topleft_y + (rel_y) * parent_size.height + y
                                                    - height);
    }

    else if (anchor == ei_anc_southeast) {
        widget->screen_location.top_left.x = old_topleft_x + (int) ((rel_x) * parent_size.width + x
                                                                    - width);
        widget->screen_location.top_left.y = old_topleft_y + (int)((rel_y) * parent_size.height + y
                                                                   - height);
    }

    else if (anchor == ei_anc_center) {
        widget->screen_location.top_left.x = (int) (old_topleft_x + (rel_x) * parent_size.width + x
                                                    - width / 2);
        widget->screen_location.top_left.y = (int) (old_topleft_y + (rel_y) * parent_size.height + y
                                                    - height / 2);
    }

    else if (anchor == ei_anc_north) {
        widget->screen_location.top_left.x = (int) (old_topleft_x + (rel_x) * parent_size.width + x
                                                    - width / 2);
        widget->screen_location.top_left.y = (int) (old_topleft_y + (rel_y) * parent_size.height + y);
    }

    else if (anchor == ei_anc_south) {
        widget->screen_location.top_left.x = (int) (old_topleft_x + (rel_x) * parent_size.width + x
                                                    - width / 2);
        widget->screen_location.top_left.y = (int) (old_topleft_y + (rel_y) * parent_size.height + y
                                                    - height);
    }

    else if (anchor == ei_anc_east) {
        widget->screen_location.top_left.x = (int) (old_topleft_x + (rel_x) * parent_size.width + x
                                                    - width);
        widget->screen_location.top_left.y = (int) (old_topleft_y + (rel_y) * parent_size.height + y
                                                    - height / 2);
    }

    else if (anchor == ei_anc_west) {
        widget->screen_location.top_left.x = (int) (old_topleft_x + (rel_x) * parent_size.width + x);
        widget->screen_location.top_left.y = (int) (old_topleft_y + (rel_y) * parent_size.height + y
                                                    - height / 2);
    }

    else if (anchor == ei_anc_none) {
        widget->screen_location.top_left.x = (int) (old_topleft_x + (rel_x) * parent_size.width + x);
        widget->screen_location.top_left.y = (int) (old_topleft_y + (rel_y) * parent_size.height + y);
    }

    widget->content_rect->top_left.x = widget->screen_location.top_left.x + constante_x ;
    widget->content_rect->top_left.y = widget->screen_location.top_left.y + constante_y ;

    ei_widget_t *current_child = widget->children_head;
    while(current_child != NULL) {
        if (current_child->geom_params != NULL &&
            strcmp(current_child->geom_params->manager->name, "placer") == 0) {

            current_child->geom_params->manager->runfunc(current_child);
        }
        current_child = current_child->next_sibling;
    }

}

void	placer_releasefunc_t	(struct ei_widget_t*	widget) {
    // TO DO
}



ei_geometrymanager_t *program_geo = NULL;
ei_geometrymanager_t placer_manager = {"placer", &placer_runfunc_t, &placer_releasefunc_t, NULL};
void			ei_place			(ei_widget_t*		widget,
                                     ei_anchor_t*		anchor,
                                     int*			x,
                                     int*			y,
                                     int*			width,
                                     int*			height,
                                     float*			rel_x,
                                     float*			rel_y,
                                     float*			rel_width,
                                     float*			rel_height)
{
    structure_placeur *placeur_a_configurer = malloc(sizeof(structure_placeur));
    // anchor
    if (anchor != NULL)                     placeur_a_configurer->anchor = *anchor;
    else if (widget->geom_params != NULL)   {placeur_a_configurer->anchor = ((structure_placeur*) widget->geom_params)->anchor;}
    else                                    placeur_a_configurer->anchor = ei_anc_northwest;
    // x
    if (x != NULL)                          placeur_a_configurer->x = *x;
    else if (widget->geom_params != NULL)   placeur_a_configurer->x = ((structure_placeur*) widget->geom_params)->x;
    else                                    placeur_a_configurer->x = 0;

    // y
    if (y != NULL)                          placeur_a_configurer->y = *y;
    else if (widget->geom_params != NULL)   placeur_a_configurer->y = ((structure_placeur*) widget->geom_params)->y;
    else                                    placeur_a_configurer->y = 0;

    // rel_x
    if (rel_x != NULL)                      placeur_a_configurer->rel_x = *rel_x;
    else if (widget->geom_params != NULL)   placeur_a_configurer->rel_x = ((structure_placeur*) widget->geom_params)->rel_x;
    else                                    placeur_a_configurer->rel_x = 0;

    // rel_y
    if (rel_y != NULL)                      placeur_a_configurer->rel_y = *rel_y;
    else if (widget->geom_params != NULL)   placeur_a_configurer->rel_y = ((structure_placeur*) widget->geom_params)->rel_y;
    else                                    placeur_a_configurer->rel_y = 0;


    // rel_width && width
    if (rel_width != NULL) {
        placeur_a_configurer->rel_width = *rel_width;
        placeur_a_configurer->width = 0;
    }
    else if (width != NULL) {
        placeur_a_configurer->rel_width = 0;
        placeur_a_configurer->width = *width;
    }
    else if (widget->requested_size.width!=0){
        placeur_a_configurer->rel_width = 0;
        placeur_a_configurer->width = widget->requested_size.width;
    }
    else if (widget->geom_params != NULL) {
        placeur_a_configurer->width = ((structure_placeur*) widget->geom_params)->width;
        placeur_a_configurer->rel_width = ((structure_placeur*) widget->geom_params)->rel_width;
    }


    // rel_height && height
    if (rel_height != NULL) {
        placeur_a_configurer->rel_height = *rel_height;
        placeur_a_configurer->height = 0;
    }
    else if (height!= NULL) {
        placeur_a_configurer->rel_height = 0;
        placeur_a_configurer->height = *height;
    }

    else if (widget->requested_size.height !=0) {
        placeur_a_configurer->rel_height = 0;
        placeur_a_configurer->height = widget->requested_size.height;
    }

    else if (widget->geom_params != NULL) {
        placeur_a_configurer->height = ((structure_placeur*) widget->geom_params)->height;
        placeur_a_configurer->rel_height = ((structure_placeur*) widget->geom_params)->rel_height;
    }




    placeur_a_configurer->manager = &placer_manager;
    widget->geom_params = (ei_geometry_param_t *) placeur_a_configurer;
    widget->geom_params->manager->runfunc(widget);

}




void 			ei_register_placer_manager 	(void)
{
    ei_geometrymanager_t *placer = malloc(sizeof(ei_geometrymanager_t));
    strcpy(placer->name, "placer");
    placer->runfunc     = &placer_runfunc_t                            ;
    placer->releasefunc = &placer_releasefunc_t                        ;
    placer->next        = NULL                                         ;
    ei_geometrymanager_register(placer)                  ;
}


ei_geometrymanager_t*	ei_geometrymanager_from_name	(ei_geometrymanager_name_t name)
{
    ei_geometrymanager_t *tmp = program_geo;
    while (tmp != NULL)
    {
        if (strcmp(tmp->name, name) == 0)
        {
            return tmp;
        }
        tmp = tmp -> next;
    }
    return NULL;
}

void			ei_geometrymanager_register	(ei_geometrymanager_t* geometrymanager)
{
    if (program_geo == NULL)
    {
        program_geo           = geometrymanager;
    }
    else
    {
        geometrymanager->next = program_geo    ;
        program_geo           = geometrymanager;
    }
}


/**
 * \brief	Tell the geometry manager in charge of a widget to forget it. This removes the
 *		widget from the screen. If the widget is not currently managed, this function
 *		returns silently.
 *		Side effects:
 *		<ul>
 *			<li> the \ref ei_geometrymanager_releasefunc_t of the geometry manager in
 *				charge of this widget is called, </li>
 *			<li> the geom_param field of the widget is freed, </li>
 *			<li> the current screen_location of the widget is invalided (which will
 *				trigger a redraw), </li>
 *			<li> the screen_location of the widget is reset to 0. </li>
 *		</ul>
 *
 * @param	widget		The widget to unmap from the screen.
 */
void			ei_geometrymanager_unmap	(ei_widget_t*		widget)
{
    widget->geom_params->manager->releasefunc(widget);
    free(widget->geom_params);
    ei_app_invalidate_rect(&widget->screen_location);
    widget->screen_location = ei_rect_zero();
}