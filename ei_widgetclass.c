#include "ei_widget.h"
#include "stdlib.h"
#include "ei_widgetclass.h"
#include "ei_frame.h"
#include "ei_button.h"
#include "ei_toplevel.h"
ei_widgetclass_t *program = NULL;
/**
 * @brief	Registers a class to the program so that widgets of this class can be created.
 *		This must be done only once per widget class in the application.
 *
 * @param	widgetclass	The structure describing the class.
 */
void			ei_widgetclass_register		(ei_widgetclass_t* widgetclass)
{
    if (program == NULL)
    {
        program = widgetclass;
    }
    else {
        widgetclass->next = program;
        program = widgetclass;
    }
}


/**
 * @brief	Returns the structure describing a class, from its name.
 *
 * @param	name		The name of the class of widget.
 *
 * @return			The structure describing the class.
 */
ei_widgetclass_t*	ei_widgetclass_from_name	(ei_widgetclass_name_t name)
{
    ei_widgetclass_t *parcours = program;
    while (parcours != NULL){
        if (strcmp(parcours->name, name) == 0){
            return parcours;
        }
        parcours = parcours -> next;
    }
    return NULL;
}

void ei_frame_register_class(void){
    ei_widgetclass_t *frame = malloc(sizeof(ei_widgetclass_t));
    strcpy(frame->name, "frame");
    frame->allocfunc = &frame_allocfunc_t;
    frame->drawfunc = &frame_drawfunc_t;
    frame->releasefunc = &frame_releasefunc_t;
    frame->setdefaultsfunc = &frame_setdefaultsfunc_t;
    frame->geomnotifyfunc = &frame_geomnotifyfunc_t;
    frame->next = NULL;
    ei_widgetclass_register(frame);
}
void ei_button_register_class(void){
    ei_widgetclass_t *button = malloc(sizeof(ei_widgetclass_t));
    strcpy(button->name, "button");
    button->allocfunc = &button_allocfunc_t;
    button->drawfunc = &button_drawfunc_t;
    button->releasefunc = &button_releasefunc_t;
    button->setdefaultsfunc = &button_setdefaultsfunc_t;
    button->geomnotifyfunc = &button_geomnotifyfunc_t;
    button->next = NULL;
    ei_bind(ei_ev_mouse_buttondown, NULL, "button", bouton_enfonce, NULL);
    ei_widgetclass_register(button);
}
void ei_toplevel_register_class(void) {
    ei_widgetclass_t *toplevel = malloc(sizeof(ei_widgetclass_t));
    strcpy(toplevel->name, "toplevel");
    toplevel->allocfunc = &toplevel_allocfunc_t;
    toplevel->drawfunc = &toplevel_drawfunc_t;
    toplevel->releasefunc = &toplevel_releasefunc_t;
    toplevel->setdefaultsfunc = &toplevel_setdefaultsfunc_t;
    toplevel->geomnotifyfunc = &button_geomnotifyfunc_t;
    toplevel->next = NULL;
    ei_bind(ei_ev_mouse_buttondown, NULL, "toplevel", debut_deplacement, NULL);
    ei_bind(ei_ev_mouse_buttondown, NULL, "toplevel", debut_redimensionnement, NULL);
    ei_widgetclass_register(toplevel);
}
