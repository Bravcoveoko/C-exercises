#include "osc.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

void OSC_BUNDLE_NULL(struct osc_bundle *bundle) {
    bundle->timetag = NULL;
    bundle->raw_data = NULL;
}

void OSC_TIMETAG_IMMEDIATE(struct osc_timetag *timetag) {
    timetag->sec = 0;
    timetag->frac = 1;
}

void OSC_TIMETAG_NULL(struct osc_timetag *timetag) {
    timetag->sec = 0;
    timetag->frac = 0;
}

void OSC_MESSAGE_NULL(struct osc_message *message) {
    message->address = NULL;
    message->typetag = NULL;
    message->raw_data = NULL;
}

void osc_message_destroy(struct osc_message *msg) {
    free(msg->raw_data);
    OSC_MESSAGE_NULL(msg);
}

int osc_message_new(struct osc_message *msg) {
    void *pData = calloc(12, sizeof(char));

    if (!pData) {
        return 1;
    }

    *((int32_t *)pData) = 8;

    char *pAddress = (char *)pData + sizeof(uint32_t);

    char *pTyptag = (char *)pData + (2 * sizeof(uint32_t));
    memset(pTyptag, ',', 1);
  
    msg->raw_data = pData;
    msg->address = pAddress;
    msg->typetag = pTyptag;

    return 0;
}

/* Function which returns lenght of string with zero's */
static size_t getLength(size_t textLenght) {
    size_t rest = textLenght % 4;
    return textLenght + (4 - rest);
}

int osc_message_set_address(struct osc_message *msg, const char *address) {
    if (address == NULL) {
        return 1;
    }
    // Length of msg->address (with zeros)
    size_t msgAdrLen = getLength(strlen(msg->address));
    // Lenght of whole message without first 4B
    int32_t msgLen = *((int32_t *)(msg->raw_data));
    // Length of given address (with zeros)
    size_t addressLen = getLength(strlen(address));
    // No memory will change if theirs size is same
    if (msgAdrLen == addressLen) {
        memmove(msg->address, address, addressLen);
        return 0;
    }
    // length of typetag + data
    size_t ttDataLen = msgLen - msgAdrLen;
    // new address + typetag + data
    int32_t newDataLength = addressLen + ttDataLen;

    // If given address is smaller than original address 
    // we have to move data, cause realloc can cut some data.
    if (addressLen < msgAdrLen) {
        size_t lenghtDifference = msgAdrLen - addressLen;
        char *pOldPos = msg->typetag;
        char *pNewPos = msg->typetag - lenghtDifference;
        memmove(pNewPos, pOldPos, ttDataLen);
        void *pData = realloc(msg->raw_data, newDataLength + sizeof(uint32_t));
        if (!pData) {
            return 1;
        }
        char *pNewAddress = ((char *)pData) + sizeof(uint32_t);
        memmove(pNewAddress, address, addressLen);

        *(int32_t *)pData = newDataLength;
        msg->raw_data = pData;
        msg->address = pNewAddress;
        msg->typetag = (char *)pData + sizeof(uint32_t) + addressLen;

        return 0;
    }
    // Otherwise we dont have to move nothing before realloc
    void *pData = realloc(msg->raw_data, newDataLength + sizeof(uint32_t));
    if (!pData) {
        return 1;
    }
    // Old position of typetag + data
    char *oldPtr = (char *)(pData) + sizeof(uint32_t) + msgAdrLen;
    // New one for typetag + data
    char *newPtr = (char *)(pData) + sizeof(uint32_t) + addressLen;
    memmove(newPtr, oldPtr, ttDataLen);
    // Copy new address
    char *newAddr = (char *)pData + sizeof(uint32_t);
    memmove(newAddr, address, addressLen);

    *((int32_t *)pData) = newDataLength;
    msg->raw_data = pData;
    msg->address = newAddr;
    msg->typetag = newPtr;

    return 0;
}

// Return value of given type
// Different with string -> return strlen
static size_t value(char oscData) {
    switch(oscData) {
        case OSC_TT_INT:
            return sizeof(int32_t);
        case OSC_TT_FLOAT:
            return sizeof(float);
        case OSC_TT_TIMETAG:
            return sizeof(struct osc_timetag);
        default:
            return 0;
    }
}

// Generic function for: osc_message_add_int32, osc_message_add_float etc .. 
static int add_new_data(char oscData, struct osc_message *msg, const void* data) {
    size_t msgAdrLen = getLength(strlen(msg->address));
    size_t msgTtLen = getLength(strlen(msg->typetag));
    size_t TtCharLen = strlen(msg->typetag);
    // Length of rest data
    int32_t restDataLen = *((int32_t *)(msg->raw_data)) - msgAdrLen - msgTtLen;
    size_t newTypeTagLen = getLength(strlen(msg->typetag) + 1);
    size_t addZeros = newTypeTagLen - msgTtLen;
    // Which type is given
    // data parameter is used only for OSC_STRING
    size_t dataToPut = (oscData == OSC_TT_STRING) ? getLength(strlen((char *)data)) : value(oscData);
    if (!dataToPut) {
        return 1;
    }

    // Length of whole message after addition
    int32_t newMsgLen = msgAdrLen + newTypeTagLen + restDataLen + dataToPut;
    // first 4B + address length + new typetag length + length of old data + length of new data
    void *pData = realloc(msg->raw_data, (sizeof(uint32_t) + newMsgLen));
     if (!pData) {
        return 1;
    }
    char *pRestData = ((char *)pData) + sizeof(uint32_t) + msgAdrLen + msgTtLen;
    char *newRestDataPos = ((char *)pData) + sizeof(uint32_t) + msgAdrLen + newTypeTagLen;
    memmove(newRestDataPos, pRestData, restDataLen);
    memset(pRestData, '\0', addZeros);
    char *pTypetag = ((char *)pData) + sizeof(uint32_t) + msgAdrLen + TtCharLen;
    memset(pTypetag, oscData, 1);

    char *pNewData = ((char *)pData) + sizeof(uint32_t) + msgAdrLen + newTypeTagLen + restDataLen;
    switch(oscData) {
        case OSC_TT_INT:
            *((int32_t *)pNewData) = *((int32_t *)data);
            break;
        case OSC_TT_FLOAT:
            *((float *)pNewData) = *((float *)data);
            break;
        case OSC_TT_TIMETAG:
            *((struct osc_timetag *)pNewData) = *((struct osc_timetag *)data);
            break;
        case OSC_TT_STRING:
            memmove(pNewData, data, strlen((char *)data) + 1);
            break;
        default:
            return 1;
    }

    *((int32_t *)(pData)) = newMsgLen;
    msg->raw_data = pData;
    msg->address = ((char *)pData) + sizeof(int32_t);
    msg->typetag = ((char *)pData) + sizeof(int32_t) + msgAdrLen;

    return 0;
}

int osc_message_add_int32(struct osc_message *msg, int32_t data) {
    return add_new_data(OSC_TT_INT, msg, &data);
}

// Same like in osc_message_add_int32 but different type are used
int osc_message_add_float(struct osc_message *msg, float data) {
    return add_new_data(OSC_TT_FLOAT, msg, &data);
}

int osc_message_add_timetag(struct osc_message *msg, struct osc_timetag tag) {
    return add_new_data(OSC_TT_TIMETAG, msg, &tag);
}

int osc_message_add_string(struct osc_message *msg, const char *data) {
    return add_new_data(OSC_TT_STRING, msg, data);
}   

size_t osc_message_argc(const struct osc_message *msg) {
    // Classic strlen .. and -1 because of ',' character
    return strlen(msg->typetag) - 1;
}

static void *getArgument(char letter, void *tmpRD) {
    size_t length = 0;
    switch(letter) {
        case OSC_TT_INT:
            tmpRD = (int32_t *)tmpRD + 1;
            return tmpRD;
        case OSC_TT_FLOAT:
            tmpRD = (float *)tmpRD + 1;
            return tmpRD;
        case OSC_TT_TIMETAG:
            tmpRD = ((struct osc_timetag *)(tmpRD)) + 1;
            return tmpRD;
        case OSC_TT_STRING:
            length = getLength(strlen((char *)tmpRD));
            tmpRD = (char *)tmpRD + length;
            return tmpRD;
        default:
            tmpRD = NULL;
            return tmpRD;
    }
}

/* Main core of this function is that I read character from typetag and move pointer forward 
by size of read typetag */
const union osc_msg_argument *osc_message_arg(const struct osc_message *msg, size_t arg_index) {
    size_t typeTagLen = strlen(msg->typetag);
    if (typeTagLen - 1 <= arg_index) {
        return NULL;
    }

    size_t index;
    // Position where data start
    void *tmpRawData = ((char*)msg->typetag) + getLength(strlen(msg->typetag));

    for (index = 0; index < arg_index; ++index) {
        // After every read tmpRawData contains different pointer
        tmpRawData = getArgument(*(msg->typetag + 1 + index), tmpRawData);
        if (!tmpRawData) {
            return NULL;
        }
    }
    return (union osc_msg_argument *)tmpRawData;
}

size_t osc_message_serialized_length(const struct osc_message *msg) {
    return *((int32_t *)(msg->raw_data));
}

int osc_bundle_new(struct osc_bundle *bnd) {
    void *pData = calloc(20, sizeof(char));

    if (!pData) {
        return 1;
    }

    *((int32_t *)pData) = sizeof(struct osc_timetag) + (8 * sizeof(char));

    char *pText = (char*)pData + sizeof(uint32_t);
    char *bundleText = "#bundle";
    memmove((char *)pText, bundleText, 7 * sizeof(char));

    struct osc_timetag *pTimeTag = (struct osc_timetag *)((char *)pText + (8 * sizeof(char)));
    struct osc_timetag timetag;
    OSC_TIMETAG_IMMEDIATE(&timetag);
    memmove(pTimeTag, &timetag, sizeof(struct osc_timetag));

    bnd->raw_data = pData;
    bnd->timetag = (struct osc_timetag *)(((char *)pData) + 12 * sizeof(char));

    return 0;
}

void osc_bundle_destroy(struct osc_bundle *bn) {
    free(bn->raw_data);
    OSC_BUNDLE_NULL(bn);
}

void osc_bundle_set_timetag(struct osc_bundle *bn, struct osc_timetag tag) {
    memmove(bn->timetag, &tag, sizeof(struct osc_timetag));
}

int osc_bundle_add_message(struct osc_bundle *bundle, const struct osc_message *msg) {
    size_t bundleLenght = *((int32_t *)bundle->raw_data);
    size_t msgLength = *((int32_t *)msg->raw_data) + sizeof(uint32_t);

    void *pData = realloc(bundle->raw_data, bundleLenght + msgLength + sizeof(uint32_t));

    if (!pData) {
        return 1;
    }

    *((int32_t *)(pData)) = bundleLenght + msgLength;

    void *pNewMessage = (void *)((char *)pData + bundleLenght + sizeof(uint32_t));
    memmove(pNewMessage, msg->raw_data, msgLength);

    bundle->raw_data = pData;
    bundle->timetag = (struct osc_timetag *)((char *)pData + (12 * sizeof(char)));

    return 0;
}

struct osc_message osc_bundle_next_message(const struct osc_bundle *bundle, struct osc_message prev) {
    // Ignoring bytes: first 4B + #bundle0 + osc_timetag
    // Used for set poiner where messages start
    size_t ignoringBytes = sizeof(uint32_t) + (8 * sizeof(char)) + sizeof(struct osc_timetag);
    // Current message <- first one or end memory position
    size_t bundleLength = *((int32_t *)(bundle->raw_data)) + sizeof(uint32_t) - ignoringBytes;
    // If there si no message
    if (bundleLength == 0) {
        struct osc_message result = {NULL, NULL, NULL};
        return result;
    }
    char *currentMsg = (char *)(bundle->raw_data) + ignoringBytes;
    if (prev.raw_data == NULL) {
        struct osc_message result;

        result.raw_data = (void *)currentMsg;
        result.address = ((char *)(currentMsg)) + sizeof(uint32_t);
        result.typetag = ((char *)(currentMsg)) + sizeof(uint32_t) + getLength(strlen(result.address));

        return result;
    }
    // Length od current message without first 4B
    size_t currentMsgLen = *((int32_t *)(currentMsg)) + sizeof(uint32_t);
    while(currentMsg != prev.raw_data) {
        // After every messagage the length has to be subtract.
        bundleLength -= currentMsgLen;
        if (bundleLength == 0) {
            struct osc_message result = {NULL, NULL, NULL};
            return result;
        }

        currentMsg = ((char *)(currentMsg)) + currentMsgLen;
        currentMsgLen = *((int32_t *)currentMsg) + sizeof(uint32_t);

    }
    // Is it last message ?
    if (bundleLength - currentMsgLen == 0) {
        struct osc_message result = {NULL, NULL, NULL};
        return result;
    }
    currentMsg = ((char *)(currentMsg)) + currentMsgLen;

    struct osc_message result;
    result.raw_data = (void *)currentMsg;
    result.address = ((char *)(currentMsg)) + sizeof(uint32_t);
    result.typetag = ((char *)(currentMsg)) + sizeof(uint32_t) + getLength(strlen(result.address));

    return result;
  
}

size_t osc_bundle_serialized_length(const struct osc_bundle *bundle) {
    return *((int32_t *)(bundle->raw_data));
}


