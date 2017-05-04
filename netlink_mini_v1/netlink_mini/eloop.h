/*
 * Event loop
 * Copyright (c) 2002-2006, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 *
 * This file defines an event loop interface that supports processing events
 * from registered timeouts (i.e., do something after N seconds), sockets
 * (e.g., a new packet available for reading), and signals. eloop.c is an
 * implementation of this interface using select() and sockets. This is
 * suitable for most UNIX/POSIX systems. When porting to other operating
 * systems, it may be necessary to replace that implementation with OS specific
 * mechanisms.
 */

#ifndef ELOOP_H
#define ELOOP_H

/**
 * ELOOP_ALL_CTX - eloop_cancel_timeout() magic number to match all timeouts
 */
#define ELOOP_ALL_CTX (void *) -1

/**
 * eloop_event_type - eloop socket event type for eloop_register_sock()
 * @EVENT_TYPE_READ: Socket has data available for reading
 * @EVENT_TYPE_WRITE: Socket has room for new data to be written
 * @EVENT_TYPE_EXCEPTION: An exception has been reported
 */
typedef enum {
	EVENT_TYPE_READ = 0,
	EVENT_TYPE_WRITE,
	EVENT_TYPE_EXCEPTION
} eloop_event_type;

/**
 * eloop_sock_handler - eloop socket event callback type
 * @sock: File descriptor number for the socket
 * @eloop_ctx: Registered callback context data (eloop_data)
 * @sock_ctx: Registered callback context data (user_data)
 */
typedef void (*eloop_sock_handler)(int sock, void *eloop_ctx, void *sock_ctx);

/**
 * eloop_event_handler - eloop generic event callback type
 * @eloop_ctx: Registered callback context data (eloop_data)
 * @sock_ctx: Registered callback context data (user_data)
 */
typedef void (*eloop_event_handler)(void *eloop_data, void *user_ctx);

/**
 * eloop_init() - Initialize global event loop data
 * Returns: 0 on success, -1 on failure
 *
 * This function must be called before any other eloop_* function.
 */
int eloop_init(void);

/**
 * eloop_register_read_sock - Register handler for read events
 * @sock: File descriptor number for the socket
 * @handler: Callback function to be called when data is available for reading
 * @eloop_data: Callback context data (eloop_ctx)
 * @user_data: Callback context data (sock_ctx)
 * Returns: 0 on success, -1 on failure
 *
 * Register a read socket notifier for the given file descriptor. The handler
 * function will be called whenever data is available for reading from the
 * socket. The handler function is responsible for clearing the event after
 * having processed it in order to avoid eloop from calling the handler again
 * for the same event.
 */
int eloop_register_read_sock(int sock, eloop_sock_handler handler,
			     void *eloop_data, void *user_data);

/**
 * eloop_unregister_read_sock - Unregister handler for read events
 * @sock: File descriptor number for the socket
 *
 * Unregister a read socket notifier that was previously registered with
 * eloop_register_read_sock().
 */
void eloop_unregister_read_sock(int sock);

/**
 * eloop_register_sock - Register handler for socket events
 * @sock: File descriptor number for the socket
 * @type: Type of event to wait for
 * @handler: Callback function to be called when the event is triggered
 * @eloop_data: Callback context data (eloop_ctx)
 * @user_data: Callback context data (sock_ctx)
 * Returns: 0 on success, -1 on failure
 *
 * Register an event notifier for the given socket's file descriptor. The
 * handler function will be called whenever the that event is triggered for the
 * socket. The handler function is responsible for clearing the event after
 * having processed it in order to avoid eloop from calling the handler again
 * for the same event.
 */
int eloop_register_sock(int sock, eloop_event_type type,
			eloop_sock_handler handler,
			void *eloop_data, void *user_data);

/**
 * eloop_unregister_sock - Unregister handler for socket events
 * @sock: File descriptor number for the socket
 * @type: Type of event for which sock was registered
 *
 * Unregister a socket event notifier that was previously registered with
 * eloop_register_sock().
 */
void eloop_unregister_sock(int sock, eloop_event_type type);

/**
 * eloop_register_event - Register handler for generic events
 * @event: Event to wait (eloop implementation specific)
 * @event_size: Size of event data
 * @handler: Callback function to be called when event is triggered
 * @eloop_data: Callback context data (eloop_data)
 * @user_data: Callback context data (user_data)
 * Returns: 0 on success, -1 on failure
 *
 * Register an event handler for the given event. This function is used to
 * register eloop implementation specific events which are mainly targeted for
 * operating system specific code (driver interface and l2_packet) since the
 * portable code will not be able to use such an OS-specific call. The handler
 * function will be called whenever the event is triggered. The handler
 * function is responsible for clearing the event after having processed it in
 * order to avoid eloop from calling the handler again for the same event.
 *
 * In case of Windows implementation (eloop_win.c), event pointer is of HANDLE
 * type, i.e., void*. The callers are likely to have 'HANDLE h' type variable,
 * and they would call this function with eloop_register_event(h, sizeof(h),
 * ...).
 */
int eloop_register_event(void *event, size_t event_size,
			 eloop_event_handler handler,
			 void *eloop_data, void *user_data);

/**
 * eloop_unregister_event - Unregister handler for a generic event
 * @event: Event to cancel (eloop implementation specific)
 * @event_size: Size of event data
 *
 * Unregister a generic event notifier that was previously registered with
 * eloop_register_event().
 */
void eloop_unregister_event(void *event, size_t event_size);


/**
 * eloop_run - Start the event loop
 *
 * Start the event loop and continue running as long as there are any
 * registered event handlers. This function is run after event loop has been
 * initialized with event_init() and one or more events have been registered.
 */
void eloop_run(void);

/**
 * eloop_terminate - Terminate event loop
 *
 * Terminate event loop even if there are registered events. This can be used
 * to request the program to be terminated cleanly.
 */
void eloop_terminate(void);

/**
 * eloop_destroy - Free any resources allocated for the event loop
 *
 * After calling eloop_destroy(), other eloop_* functions must not be called
 * before re-running eloop_init().
 */
void eloop_destroy(void);

/**
 * eloop_terminated - Check whether event loop has been terminated
 * Returns: 1 = event loop terminate, 0 = event loop still running
 *
 * This function can be used to check whether eloop_terminate() has been called
 * to request termination of the event loop. This is normally used to abort
 * operations that may still be queued to be run when eloop_terminate() was
 * called.
 */
int eloop_terminated(void);

/**
 * eloop_wait_for_read_sock - Wait for a single reader
 * @sock: File descriptor number for the socket
 *
 * Do a blocking wait for a single read socket.
 */
void eloop_wait_for_read_sock(int sock);

#endif /* ELOOP_H */

