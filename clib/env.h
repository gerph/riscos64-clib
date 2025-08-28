/*******************************************************************
 * File:        env
 * Purpose:     Program environment handlers
 * Author:      Gerph
 * Date:        2025-04-06
 ******************************************************************/

#ifndef ENV_H
#define ENV_H

typedef struct riscos_error_buffer_s {
    uint32_t        pc; /* FIXME: This is only 32bit at the moment */
    _kernel_oserror err;
} riscos_error_buffer_t;
extern riscos_error_buffer_t _kernel_error_buffer;

/*************************************************** Gerph *********
 Function:      _env_init
 Description:   Initialise our program environment handlers
 Parameters:    none
 Returns:       none
 ******************************************************************/
void _env_init(void);

/*************************************************** Gerph *********
 Function:      __env_restore
 Description:   Restore parent program environment handlers
 Parameters:    none
 Returns:       none
 ******************************************************************/
void _env_restore(void);

#endif
