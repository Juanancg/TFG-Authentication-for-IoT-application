# based https://gist.github.com/heskyji/5167567b64cb92a910a3

import hashlib
import hmac


def make_HMAC(message, key):
    key = bytes(key, 'UTF-8')
    message = bytes(message, 'UTF-8')

    digester = hmac.new(key, message, hashlib.sha256)
    signature1 = digester.hexdigest()

    return signature1


def prepare_msg(message, key):
    hmac_msg = make_HMAC(message, key)
    msg = hmac_msg + message
    return msg


def get_msg(message):
    msg_len = len(message)
    return message[64:msg_len]


def check_authentication(message, key):
    msg_len = len(message)
    if msg_len < 65:
        return 0
    else:
        hash_original = message[0:64]
        msg_original = message[64:msg_len]
        hash_to_compare = make_HMAC(msg_original, key)
        if hash_to_compare == hash_original:
            return 1  # ("Correct") *****************ESTO ES NUEVO Y HAY QUE PROBARLO
        else:
            return 0  # ("False")
        # return (msg_original)

# result = check_authentication(prepare_msg('get', 'secretKey'),'secretKey1')
# print(result)
