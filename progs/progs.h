/*! \file progs.h
 *  \brief Control the use of test and user programs
 *
 *  Configure which user programs or tests are to be executed.
 *
 *  \author   Fachbereich 5 - FH Aachen
 *  \date     2025
 *  \version  1.1
 */

// Testtasks for P2
#define TT_UNIT_INIT_SCHEDULER	20
#define TT_INIT					21
#define TT_RESUME				22
#define TT_MULTIPLE				23
#define TT_EXEC_P2              24
#define TT_IDLE					25
#define TT_ABC					26

// Testtasks for P3
#define TT_HEAP_COLLISION		30
#define TT_EXEC_P3              31
#define TT_STACK_CONSISTENCY	32
#define TT_TERMINATION			33
#define TT_YIELD				34
#define TT_SCHEDULING			35
#define TT_ISR_BENCHMARK		36

// Testtasks for P4
#define TT_HEAP_TERMINAL		40
#define TT_FREE_MAP				41
#define TT_STABILITY_PRIVATE	42
#define TT_HEAP_CLEANUP			43
#define TT_RANGE				44
#define TT_ALLOC_STRAT			45

// Testtasks for P5
#define TT_ADC_MONITOR			50
#define TT_DCF_SHOW_SIGNAL		51
#define TT_DCF_GENERATOR		52
#define TT_DCF_DECODE_TIME		53

// User programs for P5
#define USER_PROGRAM_DCF_CLOCK	4


///////////////////////////////////////////////////////////////////////////////
// Configure what program-set should be active: testtasks or your user progs
///////////////////////////////////////////////////////////////////////////////

// Set to 1 to run a test task, set to 0 to run user programs (see user_programs)
#define ENABLE_TESTTASK			1

// Enter testtask define (e.g. TT_INIT) and make sure ENABLE_TESTTASK is set to 1
#define TESTTASK  40

// Will run user_progs/user_progx.c if ENABLE_TESTTASK is set to 0
#define USER_PROGRAM			USER_PROGRAM_DCF_CLOCK

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


// You don't need to bother what's here
#if ENABLE_TESTTASK == 1
#define TESTTASK_ENABLED
#else
#define USER_PROGRAM_ENABLED
#endif
