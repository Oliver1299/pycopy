/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>

#include "py/nlr.h"
#include "py/runtime.h"

// this is a wrapper object that is turns something that has a __getitem__ method into an iterator

typedef struct _mp_obj_getitem_iter_t {
    mp_obj_base_t base;
    mp_obj_t args[3];
} mp_obj_getitem_iter_t;

STATIC mp_obj_t it_iternext(mp_obj_t self_in) {
    mp_obj_getitem_iter_t *self = self_in;
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        // try to get next item
        mp_obj_t value;
        if (self->args[1] == NULL) {
            value = mp_obj_subscr(self->args[0], self->args[2], MP_OBJ_SENTINEL);
        } else {
            value = mp_call_method_n_kw(1, 0, self->args);
        }
        self->args[2] = MP_OBJ_NEW_SMALL_INT(MP_OBJ_SMALL_INT_VALUE(self->args[2]) + 1);
        nlr_pop();
        return value;
    } else {
        // an exception was raised
        mp_obj_type_t *t = mp_obj_get_type(nlr.ret_val);
        if (t == &mp_type_StopIteration || t == &mp_type_IndexError) {
            // return MP_OBJ_STOP_ITERATION instead of raising
            return MP_OBJ_STOP_ITERATION;
        } else {
            // re-raise exception
            nlr_raise(nlr.ret_val);
        }
    }
}

STATIC const mp_obj_type_t it_type = {
    { &mp_type_type },
    .name = MP_QSTR_iterator,
    .getiter = mp_identity,
    .iternext = it_iternext,
};

// args are those returned from mp_load_method_maybe (ie either an attribute or a method)
mp_obj_t mp_obj_new_getitem_iter(mp_obj_t *args) {
    mp_obj_getitem_iter_t *o = m_new_obj(mp_obj_getitem_iter_t);
    o->base.type = &it_type;
    o->args[0] = args[0];
    o->args[1] = args[1];
    o->args[2] = MP_OBJ_NEW_SMALL_INT(0);
    return o;
}
