#include <stdlib.h>
#include "ei_application.h"
#include "ei_types.h"
#include "ei_event.h"
#include "hw_interface.h"
#include "ei_widgetclass.h"
#include "ei_frame.h"
#include "ei_button.h"
#include "ei_widget.h"

ei_surface_t ROOT_SURFACE               ;
ei_widget_t *ROOT_WIDGET          = NULL;
ei_linked_rect_t *RECT_TO_REFRESH = NULL;
ei_surface_t OFFSCREEN                  ;
ei_point_t where_from                   ;

//returns the root widget
ei_widget_t* ei_app_root_widget(void)
{
    return  ROOT_WIDGET;
}
//returns the root surface
ei_surface_t ei_app_root_surface(void)
{
    return ROOT_SURFACE;
}
//returns the offscreen
ei_surface_t ei_app_offscreen(void)
{
    return OFFSCREEN;
}
ei_bool_t QUIT = 0;

void ei_app_create(ei_size_t main_window_size, ei_bool_t fullscreen)
{
    // Init access to hardware.
    hw_init()                                                                                               ;
    ei_frame_register_class()                                                                               ;
    ei_button_register_class()                                                                              ;
    ei_toplevel_register_class()                                                                            ;

    ei_surface_t racine                      = NULL                                                         ;

    // Create the main window.

    racine                                   = hw_create_window(main_window_size, fullscreen) ;
    ROOT_SURFACE                             = racine                                                       ;

    // Allocate memory for the root widget

    ROOT_WIDGET                              = frame_allocfunc_t()                                          ;
    ROOT_WIDGET->wclass                      = ei_widgetclass_from_name	("frame")                     ;
    ROOT_WIDGET->requested_size              = main_window_size                                             ;
    ROOT_WIDGET->screen_location.top_left.x  = 0                                                            ;
    ROOT_WIDGET->screen_location.top_left.y  = 0                                                            ;
    ROOT_WIDGET->screen_location.size        = main_window_size                                             ;
    ROOT_WIDGET->requested_size              = main_window_size                                             ;
    ROOT_WIDGET->parent                      = NULL                                                         ;
    ROOT_WIDGET->content_rect                = &ROOT_WIDGET->screen_location                                ;
    ROOT_WIDGET->children_head               = NULL                                                         ;
    ROOT_WIDGET->pick_id                     = 0                                                            ;
    ROOT_WIDGET->pick_color                  =  couleur(ROOT_WIDGET->pick_id)                            ;

    // creation of the offscreen
    ei_surface_t offscreen_surface = hw_surface_create(ei_app_root_surface(), main_window_size, EI_FALSE);
    OFFSCREEN = offscreen_surface;
}

void ei_app_free(void){

    // root surface freed with hw_quit
    // free offscreen
    hw_surface_free(OFFSCREEN)             ;

    // free all widgets ..
    ei_widget_t *next_widget = NULL                ;
    ei_widget_t *widget      = ei_app_root_widget();
    while (widget)
    {
        next_widget = widget->next_sibling ;
        widget->wclass->releasefunc(widget);
        widget      = next_widget          ;
    }
}

void ei_app_run(void)
{
    ei_event_t event;
    event.type = ei_ev_none;

    // Draws in the screen for the first time
    hw_surface_lock(ei_app_root_surface());
    hw_surface_lock(ei_app_offscreen());

    draw_hierarchie(ei_app_root_widget(), OFFSCREEN, NULL);
    hw_surface_unlock(ei_app_root_surface());
    hw_surface_unlock(ei_app_offscreen());

    hw_surface_update_rects(ei_app_root_surface(), NULL);

    while (!QUIT)
    {

        // Refresh the screen only if needed
        if (RECT_TO_REFRESH )
        {

            hw_surface_lock(ei_app_root_surface());
            hw_surface_lock(ei_app_offscreen());

            ei_linked_rect_t *rect = RECT_TO_REFRESH;
            while(rect)
            {
                draw_hierarchie(ei_app_root_widget(), OFFSCREEN, NULL);
                rect = rect->next;
            }

            hw_surface_unlock(ei_app_root_surface());
            hw_surface_unlock(ei_app_offscreen());

            hw_surface_update_rects(ei_app_root_surface(), NULL);
        }
        RECT_TO_REFRESH = NULL;        // clean RECT_TO_REFRESH

        where_from = event.param.mouse.where;  // save the previous position of the mouse ( before passing to next event )

        hw_event_wait_next(&event)  ;

        if (event.type != ei_ev_none)
        {
            event_type_data *callbacks = analyse_event(&event);
            execute_callbacks(callbacks, &event);
        }


        //OK// 1.  refresh screen with hw_surface_rect_update as param : rect for rect in RECT_TO_REFRESH
        //OK// 2.  wait for an event with hw_event_wait_next(&event)
        //OK// 3.  analyse the event to find the matching callback
        //OK// 4.  call the matching callbacks (continue calling while the boolean is False)
    }
}


void ei_app_invalidate_rect(ei_rect_t* rect)
{
    // insert rect in the queue of RECT_TO_REFRESH

    struct  ei_linked_rect_t  cell                           ;
    cell.next    = RECT_TO_REFRESH ;
    struct  ei_linked_rect_t *queue        = &cell           ;

    while ( queue->next !=NULL )
    {
        queue = queue->next;
    }

    queue->next       = malloc(sizeof(struct ei_linked_rect_t));
    queue->next->rect = *rect                                       ;
    queue->next->next = NULL                                        ;
    RECT_TO_REFRESH   = cell.next                                   ;
}

void ei_app_quit_request(void)
{
    QUIT = 1;
    hw_quit();
}

ei_point_t where_from_func(void)
{
    return where_from;
}