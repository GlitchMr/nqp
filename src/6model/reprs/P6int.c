/* This is the implementation of the P6int representation, which holds a native
 * integer. */

#define PARROT_IN_EXTENSION
#include "parrot/parrot.h"
#include "parrot/extend.h"
#include "../sixmodelobject.h"
#include "P6int.h"

/* This representation's function pointer table. */
static REPROps *this_repr;

/* Creates a new type object of this representation, and associates it with
 * the given HOW. */
static PMC * type_object_for(PARROT_INTERP, PMC *HOW) {
    /* Create new object instance. */
    P6intInstance *obj = mem_allocate_zeroed_typed(P6intInstance);

    /* Build an STable. */
    PMC *st_pmc = create_stable(interp, this_repr, HOW);
    STable *st  = STABLE_STRUCT(st_pmc);

    /* Create type object and point it back at the STable. */
    obj->common.stable = st_pmc;
    st->WHAT = wrap_object(interp, obj);
    PARROT_GC_WRITE_BARRIER(interp, st_pmc);

    /* Flag it as a type object. */
    MARK_AS_TYPE_OBJECT(st->WHAT);

    return st->WHAT;
}

/* Creates a new instance based on the type object. */
static PMC * allocate(PARROT_INTERP, STable *st) {
    P6intInstance *obj = mem_allocate_zeroed_typed(P6intInstance);
    obj->common.stable = st->stable_pmc;
    return wrap_object(interp, obj);
}

/* Initialize a new instance. */
static void initialize(PARROT_INTERP, STable *st, void *data) {
    ((P6intBody *)data)->value = 0;
}

/* Copies to the body of one object to another. */
static void copy_to(PARROT_INTERP, STable *st, void *src, void *dest) {
    *((INTVAL *)dest) = *((INTVAL *)src);
}

/* Used with boxing. Sets an integer value, for representations that can hold
 * one. */
static void set_int(PARROT_INTERP, STable *st, void *data, INTVAL value) {
    ((P6intBody *)data)->value = value;
}

/* Used with boxing. Gets an integer value, for representations that can
 * hold one. */
static INTVAL get_int(PARROT_INTERP, STable *st, void *data) {
    return ((P6intBody *)data)->value;
}

/* Used with boxing. Sets a floating point value, for representations that can
 * hold one. */
static void set_num(PARROT_INTERP, STable *st, void *data, FLOATVAL value) {
    Parrot_ex_throw_from_c_args(interp, NULL, EXCEPTION_INVALID_OPERATION,
            "P6int cannot box a native num");
}

/* Used with boxing. Gets a floating point value, for representations that can
 * hold one. */
static FLOATVAL get_num(PARROT_INTERP, STable *st, void *data) {
    Parrot_ex_throw_from_c_args(interp, NULL, EXCEPTION_INVALID_OPERATION,
            "P6int cannot unbox to a native num");
}

/* Used with boxing. Sets a string value, for representations that can hold
 * one. */
static void set_str(PARROT_INTERP, STable *st, void *data, STRING *value) {
    Parrot_ex_throw_from_c_args(interp, NULL, EXCEPTION_INVALID_OPERATION,
            "P6int cannot box a native string");
}

/* Used with boxing. Gets a string value, for representations that can hold
 * one. */
static STRING * get_str(PARROT_INTERP, STable *st, void *data) {
    Parrot_ex_throw_from_c_args(interp, NULL, EXCEPTION_INVALID_OPERATION,
            "P6int cannot unbox to a native string");
}

/* Some objects serve primarily as boxes of others, inlining them. This gets
 * gets the reference to such things, using the representation ID to distinguish
 * them. */
static void * get_boxed_ref(PARROT_INTERP, STable *st, void *data, INTVAL repr_id) {
    Parrot_ex_throw_from_c_args(interp, NULL, EXCEPTION_INVALID_OPERATION,
            "P6int cannot box other types");
}

/* This Parrot-specific addition to the API is used to free an object. */
static void gc_free(PARROT_INTERP, PMC *obj) {
    mem_sys_free(PMC_data(obj));
    PMC_data(obj) = NULL;
}

/* Gets the storage specification for this representation. */
static storage_spec get_storage_spec(PARROT_INTERP, STable *st) {
    storage_spec spec;
    spec.inlineable = STORAGE_SPEC_INLINED;
    spec.bits = sizeof(INTVAL) * 8;
    spec.boxed_primitive = STORAGE_SPEC_BP_INT;
    spec.can_box = STORAGE_SPEC_CAN_BOX_INT;
    return spec;
}

/* Serializes the data. */
static void serialize(PARROT_INTERP, STable *st, void *data, SerializationWriter *writer) {
    writer->write_int(interp, writer, ((P6intBody *)data)->value);
}

/* Deserializes the data. */
static void deserialize(PARROT_INTERP, STable *st, void *data, SerializationReader *reader) {
    ((P6intBody *)data)->value = reader->read_int(interp, reader);
}

/* Initializes the P6int representation. */
REPROps * P6int_initialize(PARROT_INTERP) {
    /* Allocate and populate the representation function table. */
    this_repr = mem_allocate_zeroed_typed(REPROps);
    this_repr->type_object_for = type_object_for;
    this_repr->allocate = allocate;
    this_repr->initialize = initialize;
    this_repr->copy_to = copy_to;
    this_repr->box_funcs = mem_allocate_typed(REPROps_Boxing);
    this_repr->box_funcs->set_int = set_int;
    this_repr->box_funcs->get_int = get_int;
    this_repr->box_funcs->set_num = set_num;
    this_repr->box_funcs->get_num = get_num;
    this_repr->box_funcs->set_str = set_str;
    this_repr->box_funcs->get_str = get_str;
    this_repr->box_funcs->get_boxed_ref = get_boxed_ref;
    this_repr->gc_free = gc_free;
    this_repr->get_storage_spec = get_storage_spec;
    this_repr->serialize = serialize;
    this_repr->deserialize = deserialize;
    return this_repr;
}
