#include "ei_widget.h"
#include "ei_event.h"
#include "ei_button.h"
#include "ei_frame.h"
#include "ei_application.h"
type_managed *my_manager = NULL;
/**
 * \brief	Binds a callback to an event type and a widget or a tag.
 *
 * @param	eventtype	The type of the event.
 * @param	widget		The callback is only called if the event is related to this widget.
 *				This parameter must be NULL if the "tag" parameter is not NULL.
 * @param	tag		The callback is only called if the event is related to a widget that
 *				has this tag. A tag can be a widget class name, or the tag "all".
 *				This parameter must be NULL is the "widget" parameter is not NULL.
 * @param	callback	The callback (i.e. the function to call).
 * @param	user_param	A user parameter that will be passed to the callback when it is called.
 */
void		ei_bind			(ei_eventtype_t		eventtype,
                                ei_widget_t*		widget,
                                ei_tag_t		tag,
                                ei_callback_t		callback,
                                void*			user_param)
{
    // parcourt manager et quand tu trouve type == eventtype tu ajoute en queue du callback une case pour widget, tag, callback et user_param
    type_managed *tmp = typeManaged();
    if (typeManaged()== NULL) {
        // f(a); f(2) = 3
        // typeManaged() = cellule;
        type_managed *cellule = malloc(sizeof(type_managed));

        my_manager = cellule;
        my_manager->type = eventtype;
        my_manager->callbacks = malloc(sizeof(event_type_data));
        my_manager->callbacks->widget = widget;
        my_manager->callbacks->tag = tag;
        my_manager->callbacks->callback = callback;
        my_manager->callbacks->user_param = user_param;
        my_manager->callbacks->next = NULL;
        my_manager->next = NULL;
        return ;
    }
    else
    {
        while (tmp)
        {
            if (tmp->type == eventtype)
            {

                if (tmp->callbacks == NULL)
                {
                    event_type_data *cellule = malloc(sizeof(event_type_data));
                    cellule->widget = widget;
                    cellule->tag = tag;
                    cellule->callback = callback;
                    cellule->user_param = user_param;
                    cellule->next = NULL;
                    cellule->prec = NULL;
                    tmp->callbacks = cellule;
                    return ;
                }
                else
                {
                    ajoute_queue_event_type(&tmp->callbacks, widget, tag, callback, user_param);
                    return ;
                }/*
            ajoute_queue_event_type(&tmp->callbacks, widget, tag, callback, user_param );
            return ;*/
            }
            tmp = tmp->next;

        }
        type_managed *manager = typeManaged();
        event_type_data *callbacks = malloc(sizeof(event_type_data));
        callbacks->next = NULL;
        callbacks->callback = callback;
        callbacks->tag = tag;
        callbacks->widget = widget;
        callbacks->user_param = user_param;
        ajoute_queue_type_managed(&manager, eventtype, callbacks);
        return ;
    }


    /*
    event_type_data *callbacks = malloc(sizeof(event_type_data));
    callbacks->next = NULL;
    callbacks->callback = callback;
    callbacks->tag = tag;
    callbacks->widget = widget;
    callbacks->user_param = user_param;
    ajoute_queue_type_managed(typeManaged(), eventtype, callbacks);
    */


}

/**
 * \brief	Unbinds a callback from an event type and widget or tag.
 *
 * @param	eventtype, widget, tag, callback, user_param
 *				All parameters must have the same value as when \ref ei_bind was
 *				called to create the binding.
 */
void		ei_unbind		(ei_eventtype_t		eventtype,
                              ei_widget_t*		widget,
                              ei_tag_t		tag,
                              ei_callback_t		callback,
                              void*			user_param)
{
    // parcourt manager et quand tu trouve type == eventtype et que tout les champs matchent avec les valeur en entree tu supprime alors la cellule
    type_managed *tmp = typeManaged();
    event_type_data *tmp_callbacks = NULL;

    while (tmp)
    {
        if (tmp->type == eventtype)
        {
            tmp_callbacks = tmp->callbacks;
            while (tmp_callbacks)
            {
                if (tmp_callbacks->tag == tag && tmp_callbacks->user_param == user_param && tmp_callbacks->callback == callback && tmp_callbacks->widget == widget)
                {
                    if (tmp_callbacks != tmp->callbacks)
                    {
                        tmp_callbacks->prec->next = tmp_callbacks->next;
                        if (tmp_callbacks->next)
                        {
                            tmp_callbacks->next->prec = tmp_callbacks->prec;
                        }
                    }
                    else
                    {
                        tmp->callbacks = NULL;
                    }
                    return;
                }
                tmp_callbacks = tmp_callbacks->next;
            }
            break;
        }
        tmp = tmp->next;
    }

}
event_type_data *analyse_event(ei_event_t *event)
{
    type_managed *tmp = typeManaged();

    while (tmp)
    {
        if (tmp->type == event->type)
        {

            return tmp->callbacks; // pointe sur le premier callback de ce type d'evenement
        }
        tmp = tmp->next;
    }
}

// executes all the callbacks pointed at by callbacks paramter concerning the event given
void execute_callbacks(event_type_data *callbacks, ei_event_t *event)
{
    ei_bool_t ARRET = EI_FALSE;
    while (!ARRET && callbacks != NULL)
    {
        if (callbacks->widget)
        {
            if (ei_widget_pick(&event->param.mouse.where) ==  callbacks->widget)
            {
                ARRET = callbacks->callback(callbacks->widget, event, callbacks->user_param);
                ei_app_invalidate_rect(&callbacks->widget->screen_location);
            }
        }
        else
        {
            if (strcmp(callbacks->tag, "all") == 0)
            {
                // parcours de toute la hierarchie pour recuperer widget
                ARRET = parcourt_hierarchie(ei_app_root_widget(), "all", ARRET, callbacks, event);
                /*while (next_widget("all")!=NULL && !ARRET)
                {
                    ARRET = callbacks->callback(next_widget("all"), event, callbacks->user_param);
                }*/
                // appel callback sur  toute la hierarchie
            }
            else if (strcmp(callbacks->tag, "button") == 0)
            {
                ARRET = parcourt_hierarchie(ei_app_root_widget() ,"button", ARRET, callbacks, event);

                // parcourt toute la hierarchie et appel callback sur tous les buttons
                /*
                while (next_widget("button")!=NULL && !ARRET)
                {
                    ARRET = callbacks->callback(next_widget("button"), event, callbacks->user_param);
                }
                */
            }
            else if (strcmp(callbacks->tag, "frame") == 0)
            {
                ARRET = parcourt_hierarchie(ei_app_root_widget() , "frame", ARRET, callbacks, event);

                // parcourt toute la hierarchie et appel callback sur tous les frames
                /*
                while (next_widget("frame")!=NULL && !ARRET)
                {
                    ARRET = callbacks->callback(next_widget("frame"), event, callbacks->user_param);
                }
                 */
            }
            else if (strcmp(callbacks->tag, "toplevel") == 0)
            {
                ARRET = parcourt_hierarchie(ei_app_root_widget() , "toplevel", ARRET, callbacks, event);

                // parcourt toute la hierarchie et appel callback sur tous les toplevels
                /*
                while (next_widget("toplevel")!=NULL && !ARRET)
                {
                    ARRET = callbacks->callback(next_widget("toplevel"), event, callbacks->user_param);
                }
                 */
            }
        }
        callbacks = callbacks->next;
    }
}

// returns our global variable manager
type_managed *typeManaged(void)
{
    return my_manager;
}

//adds an the given_type data given to the event type data chained list
void ajoute_queue_event_type(event_type_data **pl,
                             ei_widget_t*		widget,
                             ei_tag_t		tag,
                             ei_callback_t		callback,
                             void*			user_param)
{
    struct event_type_data cell ;
    cell.next = *pl;
    struct event_type_data *queue = &cell;

    while (queue->next !=NULL)
    {
        queue = queue->next;
    }
    queue->next = malloc(sizeof(struct event_type_data));
    queue->next->tag = tag;
    queue->next->widget = widget;
    queue->next->callback= callback;
    queue->next->user_param= user_param;
    queue->next->prec = queue;
    queue->next->next = NULL;
    *pl = cell.next;

    /*
    else
    {
        *pl = malloc(sizeof(event_type_data));
        (*pl)->widget = widget;
        (*pl)->tag = tag;
        (*pl)->callback = callback;
        (*pl)->user_param = user_param;
        (*pl)->next = NULL;
    }*/

}

// adds the event type to the chained list of event type
void ajoute_queue_type_managed(type_managed **pl,ei_eventtype_t eventtype, event_type_data *callbacks)
{
    if (1)
    {
        struct type_managed cell ;
        cell.next = *pl;
        struct type_managed *queue = &cell;

        while (queue->next !=NULL)
        {
            queue = queue->next;
        }
        queue->next = malloc(sizeof(struct type_managed));
        queue->next->type = eventtype;
        queue->next->callbacks = callbacks;
        queue->next->next = NULL;
        *pl = cell.next;
    }
    /*
    else
    {
        pl = malloc(sizeof(type_managed));
        (*pl)->type = eventtype;
        (*pl)->callbacks = callbacks;
        (*pl)->next = NULL;
    }
     */

}

// goes through every created widget in our 'tree'
ei_bool_t parcourt_hierarchie(ei_widget_t *widget, ei_tag_t tag, ei_bool_t eiBool, event_type_data *callbacks, ei_event_t *event)
{
    if (widget)
    {
        if (strcmp(widget->wclass->name, tag)==0 || strcmp("all", tag)==0)
        {
            eiBool = callbacks->callback(widget, event, callbacks->user_param);
            if (eiBool == EI_TRUE)
            {
                return EI_TRUE;
            }
        }
        ei_widget_t *tmp_fils = widget->children_head;
        while (tmp_fils != NULL)
        {
            parcourt_hierarchie(tmp_fils, tag, eiBool, callbacks, event);
            tmp_fils = tmp_fils->next_sibling;
        }
    }
}


