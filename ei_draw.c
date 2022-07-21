#include "ei_widget.h"
#include "ei_widgetclass.h"
#include "hw_interface.h"
#include "math.h"
#include "ei_button.h"
#include "ei_frame.h"
#include "ei_application.h"
#include "ei_utils.h"
int min(int a, int b);
int max(int a, int b);
/**

 * @param	destination	The surface on which to copy pixels.
 * @param	dst_rect	If NULL, the entire destination surface is used. If not NULL,
 *				defines the rectangle on the destination surface where to copy
 *				the pixels.
 * @param	source		The surface from which to copy pixels.
 * @param	src_rect	If NULL, the entire source surface is used. If not NULL, defines the
 *				rectangle on the source surface from which to copy the pixels.
 * @param	alpha		If true, the final pixels are a combination of source and
 *				destination pixels weighted by the source alpha channel and
 *				the transparency of the final pixels is set to opaque.
 *				If false, the final pixels are an exact copy of the source pixels,
 				including the alpha channel.
 *
 * @return			Returns 0 on success, 1 on failure (different sizes between source and destination).
 */

// a revoir
int			ei_copy_surface		(ei_surface_t		destination,
                                       const ei_rect_t*	dst_rect,
                                       ei_surface_t		source,
                                       const ei_rect_t*	src_rect,
                                       ei_bool_t		alpha)
{

    uint8_t  *pixel_des ;
    if (dst_rect){pixel_des = hw_surface_get_buffer(destination) + (dst_rect->top_left.y) * hw_surface_get_size(destination).width*4 + dst_rect->top_left.x*4;}
    else
    {
        pixel_des = hw_surface_get_buffer(destination);
    }
    uint8_t *pixel_src;
    int hauteur;
    int largeur;
    if (src_rect){
        pixel_src = hw_surface_get_buffer(source) + (src_rect->top_left.y) * hw_surface_get_size(source).width*4 + src_rect->top_left.x*4;
        hauteur   = src_rect->size.height;
        largeur = src_rect->size.width;
    }
    else {
        pixel_src = hw_surface_get_buffer(source);
        hauteur   = hw_surface_get_size(source).height;
        largeur   = hw_surface_get_size(source).width;
    }

    for (uint32_t j = 0; j < hauteur; j++)
    {
        for (uint32_t i = 0; i < largeur; i++)
        {
            if (alpha == EI_TRUE)
            {
                pixel_des[3] = pixel_src[3];
                pixel_des[2] = (pixel_src[3] * pixel_src[2] + (255 - pixel_src[3]) * pixel_des[2])/255;
                pixel_des[1] = (pixel_src[3] * pixel_src[1] + (255 - pixel_src[3]) * pixel_des[1])/255;
                pixel_des[0] = (pixel_src[3] * pixel_src[0] + (255 - pixel_src[3]) * pixel_des[0])/255;
            }
            else if (alpha == EI_FALSE)
            {
                pixel_des[3] = pixel_src[3];
                pixel_des[2] = pixel_src[2];
                pixel_des[1] = pixel_src[1];
                pixel_des[0] = pixel_src[0];
            }
            pixel_src = pixel_src + 4;
            pixel_des = pixel_des + 4;
        }
        pixel_src = pixel_src + 4*(hw_surface_get_size(source).width - largeur);
        pixel_des = pixel_des + 4*(hw_surface_get_size(destination).width - largeur);
    }
    return 0;
}


void			ei_draw_text		(ei_surface_t		surface,
                                     const ei_point_t*	where,
                                     const char*		text,
                                     ei_font_t		font,
                                     ei_color_t		color,
                                     const ei_rect_t*	clipper)
{
    ei_surface_t  surface_source = hw_text_create_surface(text, font, color);
    ei_size_t size_image = hw_surface_get_size(surface_source);
    ei_rect_t rect_dest ;

    ei_rect_t src_rect ;
    src_rect.top_left = ei_point_zero();
    if (clipper != NULL)
    {
        rect_dest.size = ei_size(min(size_image.width, clipper->size.width), min(size_image.width, clipper->size.width)) ;
        rect_dest.top_left.x = max(clipper->top_left.x, where->x);
        rect_dest.top_left.y = max(clipper->top_left.y, where->y);
        if (where->x < (*clipper).top_left.x)
        {
            src_rect.top_left.x = (*clipper).top_left.x - where->x;
        }
        if (where->y < (*clipper).top_left.y)
        {
            src_rect.top_left.y = (*clipper).top_left.y - where->y;
        }
    }
    else
    {
        rect_dest.size = size_image;
        rect_dest.top_left = *where;
    }
    src_rect.size = rect_dest.size; // the src_rect ' s take the size of the rect_dest

    // rect_dest est l'intersection du rectangle clipper et du rectangle necessaire pour ecrire le texte, il a
    // pour top_left *where
    // src_rect is a rect with the same size as rect_dest but with a zero_point topleft.

    ei_copy_surface(surface, &rect_dest, surface_source, NULL, EI_TRUE);
    hw_surface_free(surface_source);
}

void			ei_fill			(ei_surface_t		surface,
                                    const ei_color_t*	color,
                                    const ei_rect_t*	clipper)
{

    uint32_t* ptr;
    if (clipper == NULL)
    {
        ptr = (uint32_t*) hw_surface_get_buffer(surface);
        for (int i = 0; i< hw_surface_get_size(surface).width * hw_surface_get_size(surface).height; i++)
        {
            ptr[i] = ei_map_rgba(surface, *color);
        }
    }

    ei_rect_t rect = hw_surface_get_rect(surface);
    ei_app_invalidate_rect(&rect);

}

uint32_t		ei_map_rgba		(ei_surface_t surface, ei_color_t color)
{
    // Where to store the resulting indices.

    int ir = 0 ;
    int ig = 0;
    int ib = 0;
    int ia = 0;
    hw_surface_get_channel_indices(surface, &ir, &ig, &ib, &ia);
    uint32_t couleur;
    // uint8_t or unsigned pareil ..
    couleur = (int) ((uint8_t)color.red* pow(2, ir) + (uint8_t)color.blue* pow(2, ib) + (uint8_t)color.green * pow(2, ig));
    if (ia != -1) // hw_surface_has_alpha(surface) == EI_TRUE
    {
        couleur += (int) ((uint8_t)color.alpha * pow(2, ia)) ;
    }
    return couleur;
}

// returns the min of two given values
int min(int a, int b)
{
    if (a < b)
    {
        return a;
    }
    else
    {
        return b;
    }
}
// returns the max of two given values
int max(int a, int b)
{
    if (a < b)
    {
        return b;
    }
    else
    {
        return a;
    }
}