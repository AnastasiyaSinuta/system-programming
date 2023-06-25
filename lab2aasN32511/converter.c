#include <stdio.h> 
#include <math.h> 
 
void rgb_to_hsl(int r, int g, int b, int* h, int* s, int* l) { 
    float rr = fmaxf(0, fminf(r / 255.0f, 1.0f)); 
    float gg = fmaxf(0, fminf(g / 255.0f, 1.0f)); 
    float bb = fmaxf(0, fminf(b / 255.0f, 1.0f)); 
 
    float max = fmaxf(fmaxf(rr, gg), bb); 
    float min = fminf(fminf(rr, gg), bb); 
    float diff = max - min; 
 
    // Calculate H 
    if (diff == 0) { 
        *h = 0;  // H is undefined, set to 0 
    } else if (max == rr) { 
        *h = (int)roundf(60 * fmodf(((gg - bb) / diff), 6.0f)); 
    } else if (max == gg) { 
        *h = (int)roundf(60 * (((bb - rr) / diff) + 2)); 
    } else if (max == bb) { 
        *h = (int)roundf(60 * (((rr - gg) / diff) + 4)); 
    } 
 
    // Calculate L 
    *l = (int)roundf(((max + min) / 2) * 100); 
 
    // Calculate S 
    if (diff == 0) { 
        *s = 0; 
    } else { 
        *s = (int)roundf(diff / (1 - fabsf(2 * (*l / 100.0f) - 1)) * 100); 
    } 
} 
 
void hsl_to_rgb(int h, int s, int l, int* r, int* g, int* b) { 
    float hh = fmaxf(0, fminf(h / 360.0f, 1.0f)); 
    float ss = fmaxf(0, fminf(s / 100.0f, 1.0f)); 
    float ll = fmaxf(0, fminf(l / 100.0f, 1.0f)); 
 
    float c = (1 - fabsf(2 * ll - 1)) * ss; 
    float x = c * (1 - fabsf(fmodf(hh / 60.0f, 2) - 1)); 
    float m = ll - c / 2; 
 
    float rr, gg, bb; 
 
    if (hh >= 0 && hh < 1) { 
        rr = c; 
        gg = x; 
        bb = 0; 
    } else if (hh >= 1 && hh < 2) { 
        rr = x; 
        gg = c; 
        bb = 0; 
    } else if (hh >= 2 && hh < 3) { 
        rr = 0; 
        gg = c; 
        bb = x; 
    } else if (hh >= 3 && hh < 4) { 
        rr = 0; 
        gg = x; 
        bb = c; 
    } else if (hh >= 4 && hh < 5) { 
        rr = x; 
        gg = 0; 
        bb = c; 
    } else if (hh >= 5 && hh <= 6) { 
        rr = c; 
        gg = 0; 
        bb = x; 
    } 
 
    *r = (int)roundf((rr + m) * 255); 
    *g = (int)roundf((gg + m) * 255); 
    *b = (int)roundf((bb + m) * 255); 
} 
 
int main() { 
    int r = 255; 
    int g = 0; 
    int b = 0; 
    int h, s, l; 
 
    rgb_to_hsl(r, g, b, &h, &s, &l); 
    printf("RGB: %d %d %d\n", r, g, b); 
    printf("HSL: %d %d %d\n", h, s, l); 
 
    hsl_to_rgb(h, s, l, &r, &g, &b); 
    printf("HSL: %d %d %d\n", h, s, l); 
    printf("RGB: %d %d %d\n", r, g, b); 
 
    return 0; 
}
