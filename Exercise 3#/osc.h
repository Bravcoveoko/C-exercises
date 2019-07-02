#ifndef PB071_OSC_H
#define PB071_OSC_H

#include <stdint.h>
#include <stddef.h>

#define OSC_TT_INT 'i'
#define OSC_TT_STRING 's'
#define OSC_TT_FLOAT 'f'
#define OSC_TT_TIMETAG 't'

#define OSC_TYPETAG(...) {',', __VA_ARGS__, '\0'}

struct osc_timetag {
  uint32_t sec;
  uint32_t frac;
};

struct osc_message {
  char *address;
  char *typetag;
  void *raw_data;
};

struct osc_bundle {
  struct osc_timetag *timetag;
  void *raw_data;
};

union osc_msg_argument {
  char const s;
  int32_t const i;
  float const f;
  struct osc_timetag const t;
};

void OSC_BUNDLE_NULL(struct osc_bundle *bundle);

void OSC_TIMETAG_IMMEDIATE(struct osc_timetag *timetag);

void OSC_TIMETAG_NULL(struct osc_timetag *timetag);

void OSC_MESSAGE_NULL(struct osc_message *message);

void osc_message_destroy(struct osc_message *msg);

int osc_message_new(struct osc_message *msg);

int osc_message_set_address(struct osc_message *msg, const char *address);

int osc_message_add_int32(struct osc_message *msg, int32_t data);

int osc_message_add_float(struct osc_message *msg, float data);

int osc_message_add_timetag(struct osc_message *msg, struct osc_timetag tag);

int osc_message_add_string(struct osc_message *msg, const char *data);

size_t osc_message_argc(const struct osc_message *msg);

const union osc_msg_argument *osc_message_arg(const struct osc_message *msg, size_t arg_index);

size_t osc_message_serialized_length(const struct osc_message *msg);

int osc_bundle_new(struct osc_bundle *bnd);

void osc_bundle_destroy(struct osc_bundle *bn);

void osc_bundle_set_timetag(struct osc_bundle *bn, struct osc_timetag tag);

int osc_bundle_add_message(struct osc_bundle *bundle, const struct osc_message *msg);

struct osc_message osc_bundle_next_message(const struct osc_bundle *bundle, struct osc_message prev);

size_t osc_bundle_serialized_length(const struct osc_bundle *bundle);
#endif // PB071_OSC_H

