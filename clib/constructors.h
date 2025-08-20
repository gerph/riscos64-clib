/*******************************************************************
 * File:        constructors
 * Purpose:     Invocation of the constructors and destructors
 * Author:      Gerph
 * Date:        18 Aug 2025
 ******************************************************************/

#ifndef CONSTRUCTORS_H
#define CONSTRUCTORS_H

/*************************************************** Gerph *********
 Function:      __constructors_init
 Description:   Invoke all of the constructor functions
 Parameters:    none
 Returns:       none
 ******************************************************************/
void __constructors_init(void);

/*************************************************** Gerph *********
 Function:      __constructors_init
 Description:   Invoke all of the destructor functions
 Parameters:    none
 Returns:       none
 ******************************************************************/
void __constructors_final(void);

#endif
