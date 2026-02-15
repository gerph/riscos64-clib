#include <stdarg.h>

typedef struct outputter_s {
    int (*write0)(struct outputter_s *out, const char *str);
    int (*writec)(struct outputter_s *out, char c);
    int (*writen)(struct outputter_s *out, const char *str, size_t len);
    int (*newline)(struct outputter_s *out);
    void *private;
} outputter_t;

typedef struct formatparams_s {
    int alternate;
    int sign;
    int digit_pad;
    int align_left;
    int field_width;
    int precision;
    int param_width;
} formatparams_t;

int _vprintf(outputter_t *out, const char *format, va_list args);
