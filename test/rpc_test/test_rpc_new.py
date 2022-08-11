# -*-encoding: utf-8-*-
from pickle import FALSE
import unittest
import json
import requests
import re
import string
import types

URL = "http://192.168.109.194:8765"
#0xC98A676DE3E0C539742E3023F7755C57E331E42F
#Judge account number, mcp_start, remove I, O, l, 0, length equal to 42	
def is_account(str):
    if re.findall(r'0x[0123456789ABCDEFabcdef]{40,}',str):
        return True
    else:
        return False

#judge gas
def is_gas(str):
    if str.isdigit():
        return True
    else:
        return False

#judge balancd
def is_balance(str):
    if str.isdigit():
        return True
    else:
        return False

#judge hex
def is_hex(str, is_lens=None):
    # if is_lens is None:
    # 	if re.findall(r"^[A-F0-9]*$",str) and len(str) % 2 == 0:
    # 		return True
    # 	else:
    # 		return False
    # else:
    # 	if re.findall(r"^[A-F0-9]{{{0}}}$".format(is_lens),str):
    # 		return True
    # 	else:
    # 		return False
    if str.lower()[:2] == '0x':
        return all(c in string.hexdigits for c in str[2:]) and (len(str) == is_lens or is_lens is None)
    else:
        return all(c in string.hexdigits for c in str) and (len(str) == is_lens or is_lens is None)

#judge signature		
def is_signature(str):
    if is_hex(str,130):
        return True
    else:
        return False

#judge str
def is_str(i):
    if re.findall("\d+(\.\d+)?",i):
        return True
    else:
        return False

#judge json
def try_load_json(jsonstr):
    try:
        json_data = json.loads(jsonstr)
        return True, json_data
    except ValueError as e:
        return False, None

#judge exist
def is_existed(json_value, key):
    if key in json_value.keys():
        return True
    else:
        return False
def is_boolean(value):
    return type(value) == bool

class Test_rpc(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        Test_rpc.genesis_account = "0x1144B522F45265C2DFDBAEE8E324719E63A1694C"
        # Test_rpc.import_account = "0xaa98fd8298939186440dd9c32f716a0b0cec376b"
        Test_rpc.import_account = "0xc165dc907fc7cb3a3748f9f2b66ef8b4cd64a70e"
        Test_rpc.import_password = "12345678"
        Test_rpc.to_account = "0xC63BE4C25041F761C5E8D9AA73FEFC57E4AA655B"
        Test_rpc.block_hash = "0xb56a5621e7c6c16d7a950afb2d578af2c660b0e04140f04a5cdad6a4cb0b172d"
        Test_rpc.transaction_hash = "0xd2df4672d8957825b2b8e832fba14498d449a4e4d5a42c409f6e9c7ab9764446"
        Test_rpc.block_number = "0x9"
    '''
    {
    "code": 0,
    "msg": "OK",
    }
    '''
    def test_account_import(self):
        data = {
            "action": "account_import",
            "json": "{\"account\":\"0xc165dc907fc7cb3a3748f9f2b66ef8b4cd64a70e\",\"kdf_salt\":\"f6022cf188a3af56ed2bd2b95c1bf941\",\"iv\":\"0f054d93ff7f6f3edfc957e53c9f4bc0\",\"ciphertext\":\"97ccc4181197970716ac25ba128688274dcfedd05cc7253982698e477834b25b\"}"
        }

        bad_data = {
            "action": "account_import",
            "json": "{\"account\":\"0x0928d83c151303fb4d4b8e465162ea02ce28ab9c\",\"kdf_salt\":\"8bfdb1afb8085ab9e237ab177eca5ee0\",\"iv\":\"6763678a6545c2b5409dae4946ba1e8b\",\"ciphertext\":\"e2d666ac15af1f99b471f904e34534afc6e549e72d830156ba21910809ff0cbd\"}"
        }

        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)

        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)

        self.assertEqual(json_data['code'], 0, json_data['msg'])
        self.assertEqual(json_data['msg'], 'OK', json_data['code'])

        print(json_data)
        print("\n")

        response = requests.post(url=URL, data=json.dumps(bad_data))
        self.assertEqual(response.status_code, 200)

        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)

        self.assertEqual(json_data['code'], 0, json_data['msg'])
        self.assertEqual(json_data['msg'], 'OK', json_data['code'])

    '''
    {
    "code": 0,
    "msg": "OK",
    "account": "0x38369682E93F5C20A4DA3E374805C9584EA3E2C8"
    }
    '''
    def test_account_create(self):
        data = {
            "action": "account_create",
            "password": Test_rpc.import_password
        }
        bad_data = {
            "action": "account_create",
            "password": ""
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])
        json_account = json_data['account']
        self.assertTrue(is_account(json_account), json_account)

        print(json_data)
        print("\n")

        response = requests.post(url=URL, data=json.dumps(bad_data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        self.assertEqual(json_data['code'], 46, json_data['msg'])

        print(json_data)
        print("\n")

    '''
    {
    "code": 0,
    "msg": "OK",
    "balances": {
        "0x38369682E93F5C20A4DA3E374805C9584EA3E2C8": "1000000000000000000",
        }
    }
    '''
    def test_accounts_balances(self):
        data = {
            "action": "accounts_balances",
            "accounts": [
                Test_rpc.genesis_account,
                Test_rpc.import_account,
                Test_rpc.to_account,
            ]
        }
        bad_datas = [
            {
                "action": "accounts_balances",
                "accounts": [
                    Test_rpc.genesis_account,
                    Test_rpc.import_account,
                    "38369682E93F5C20A4DA3E374805C9584EA3E2C800",
                ]
            },
            {
                "action": "accounts_balances",
                "accountsssss": [
                    Test_rpc.genesis_account,
                    Test_rpc.import_account,
                    "38369682E93F5C20A4DA3E374805C9584EA3E2C800",
                ]
            }
        ]

        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])
        json_balances = json_data['balances']
        self.assertTrue(len(json_balances) > 0, json_balances)
        for i in json_balances:
            self.assertTrue(is_balance(i), json_balances)

        print(json_data)
        print("\n")

        for bad_data in bad_datas:
            response = requests.post(url=URL, data=json.dumps(bad_data))
            self.assertEqual(response.status_code, 200)
            is_json, json_data = try_load_json(response.text)
            self.assertTrue(is_json, response.text)
            json_data = json.loads(response.text)
            self.assertEqual(json_data['code'], 1, json_data['msg'])
            print(json_data)
            print("\n")

    '''
    {
    "code": 0,
    "msg": "OK",
    "balance": "1000000000000000000"
    }
    '''
    def test_account_balance(self):
        data = {
            "action": "account_balance",
            "account": Test_rpc.genesis_account
        }
        bad_datas = [
            {
                "action": "account_balance",
                "accountsss": Test_rpc.genesis_account
            },
            {
                "action": "account_balance",
                "account": "123445212"
            }
        ]
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])
        json_balance = json_data['balance']
        self.assertTrue(is_balance(json_balance), json_balance)

        print(json_data)
        print("\n")

        for bad_data in bad_datas:
            response = requests.post(url=URL, data=json.dumps(bad_data))
            self.assertEqual(response.status_code, 200)
            is_json, json_data = try_load_json(response.text)
            self.assertTrue(is_json, response.text)
            json_data = json.loads(response.text)
            self.assertEqual(json_data['code'], 1, json_data['msg'])
            print(json_data)
            print("\n")

    '''
    {
    "jsonrpc": "2.0",
    "id": 1,
    "result": "0x"
    }
    '''
    def test_eth_call(self):
        data = {
            "method": "eth_call",
            "id": 1,
            "jsonrpc": "2.0",
            "params": [{
                "from": Test_rpc.import_account,
                "to": Test_rpc.to_account,
                "value": 0x186a0
            }, "latest"]
        }
        bad_datas = [
            {
                "method": "eth_call",
                "id": 1,
                "jsonrpc": "2.0",
                "params": [{
                    "from": Test_rpc.import_account,
                    "to": Test_rpc.to_account,
                    "value": 0x186a0
                }]
            }
        ]
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertFalse(is_existed(json_data, 'error'), "No error")
        print(json_data)
        print("\n")

        for bad_data in bad_datas:
            response = requests.post(url=URL, data=json.dumps(bad_data))
            self.assertEqual(response.status_code, 200)
            is_json, json_data = try_load_json(response.text)
            self.assertTrue(is_json, response.text)
            json_data = json.loads(response.text)
            self.assertTrue(is_existed(json_data, 'error'), "No error")
            print(json_data)
            print("\n")


    '''
    {
    "code": 0,
    "msg": "OK",
    "account_code": "61016B610030600B82828239805160001A6073146000811461002057610022565BFE5B5030600052607381538281F3FE7300000000000000000000000000000000000000003014608060405260043610610052576000357C010000000000000000000000000000000000000000000000000000000090048063DCE4A44714610057575B600080FD5B6100996004803603602081101561006D57600080FD5B81019080803573FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF169060200190929190505050610114565B6040518080602001828103825283818151815260200191508051906020019080838360005B838110156100D95780820151818401526020810190506100BE565B50505050905090810190601F1680156101065780820380516001836020036101000A031916815260200191505B509250505060405180910390F35B6060813B6040519150601F19601F602083010116820160405280825280600060208401853C5091905056FEA165627A7A7230582027C83370D70C11D17B94A12CCB8F7856B88ED68CFC6363465293981CB633B25C0029"
    }
    '''
    def test_account_code(self):
        data = {
            "action": "account_code",
            "account": Test_rpc.import_account
        }
        bad_data = {
            "action": "account_code",
            "account": "123123434"
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])
        json_account_code = json_data['account_code']
        self.assertTrue(is_hex(json_account_code), json_account_code)

        response = requests.post(url=URL, data=json.dumps(bad_data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 1, json_data['msg'])

    '''
    {
    "code": 0,
    "msg": "OK"
    }
    '''
    def test_account_lock(self):

        data = {
            "action": "account_lock",
            "account": Test_rpc.genesis_account
        }
        bad_data_1 = {
            "action": "account_lock",
            "account": "1231231"
        }
        bad_data_2 = {
            "action": "account_lock",
            "account": "0xb340033e234a0b5f259c79545c47e43ad6efbd00"
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

        response = requests.post(url=URL, data=json.dumps(bad_data_1))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 1, json_data['msg'])

        response = requests.post(url=URL, data=json.dumps(bad_data_2))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 7, json_data['msg'])

    '''
    {
    "code": 0,
    "msg": "OK"
    }
    '''
    def test_account_unlock(self):
        data = {
            "action": "account_unlock",
            "account": Test_rpc.genesis_account,
            "password": Test_rpc.import_password
        }

        invalid_password_data = {
            "action": "account_unlock",
            "account": Test_rpc.genesis_account
        }

        wrong_password_data = {
            "action": "account_unlock",
            "account": Test_rpc.genesis_account,
            "password": "wrong password"
        }
        not_existed_data = {
            "action": "account_unlock",
            "account": "0x1876fefb200625cf41e2d3519a5459d1558890a5",
            "password": Test_rpc.import_password
        }

        bad_data = {
            "action": "account_unlock",
            "account": "0x1876fefb200625cf41e2d3519a5459d1558890a51",
            "password": Test_rpc.import_password
        }

        lock_data = {
            "action": "account_lock",
            "account": Test_rpc.genesis_account
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json,json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

        response = requests.post(url=URL, data=json.dumps(invalid_password_data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 5, json_data['msg'])

        response = requests.post(url=URL, data=json.dumps(wrong_password_data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 8, json_data['msg'])

        response = requests.post(url=URL, data=json.dumps(not_existed_data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 7, json_data['msg'])

        response = requests.post(url=URL, data=json.dumps(bad_data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 1, json_data['msg'])

        response = requests.post(url=URL, data=json.dumps(lock_data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

    '''
    {
    "code": 0,
    "msg": "OK",
    "json": "{\"account\":\"0xaa98fd8298939186440dd9c32f716a0b0cec376b\",\"kdf_salt\":\"0a31f8d656aad9917e4d3fb83dd2662b\",\"iv\":\"f3b9f8b305d2aa39d32994c17d05e3e6\",\"ciphertext\":\"61560379be51fdfa998c198db2b685a4b14d225746729d7319c5cbc10ba28f6a\"}"
    }
    '''
    def test_account_export(self):
        data = {
            "action": "account_export",
            "account": Test_rpc.genesis_account
        }

        not_existed_data = {
            "action": "account_unlock",
            "account": "0x1876fefb200625cf41e2d3519a5459d1558890a5",
            "password": Test_rpc.import_password
        }

        bad_data = {
            "action": "account_unlock",
            "account": "0x1876fefb200625cf41e2d3519a5459d1558890a51",
            "password": Test_rpc.import_password
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])
        data_json = json_data['json']

        try:
            json_object = json.loads(data_json)
        except ValueError as e:
            self.assertTrue(False, e, data_json)

        json_account = json_object['account']
        self.assertTrue(is_account(json_account), json_account)

        json_kdf_salt = json_object['kdf_salt']
        self.assertTrue(is_hex(json_kdf_salt, 32), json_kdf_salt)

        json_iv = json_object['iv']
        self.assertTrue(is_hex(json_iv, 32), json_iv)

        json_ciphertext = json_object['ciphertext']
        self.assertTrue(is_hex(json_ciphertext, 64), json_ciphertext)

        response = requests.post(url=URL, data=json.dumps(not_existed_data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 7, json_data['msg'])

        response = requests.post(url=URL, data=json.dumps(bad_data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 1, json_data['msg'])

    '''
    {
    "code": 0,
    "msg": "OK",
    "valid": 1
    }
    '''
    def test_account_validate(self):
        data = {
            "action": "account_validate",
            "account": Test_rpc.import_account
        }

        bad_data = {
            "action": "account_validate",
            "account": "0x999999"
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json,json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])
        self.assertEqual(json_data['valid'], 1)

        response = requests.post(url=URL, data=json.dumps(bad_data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])
        self.assertEqual(json_data['valid'], 0)
    '''
    {
    "code": 0,
    "msg": "OK"
    }  
    '''
    def test_account_password_change(self):
        new_password = "qwer1234"
        wrong_password = {
            "action": "account_password_change",
            "account": Test_rpc.import_account,
            "old_password": new_password,
            "new_password": new_password
        }
        response = requests.post(url=URL, data=json.dumps(wrong_password))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 8, json_data['msg'])

        invalid_old_password = {
            "action": "account_password_change",
            "account": Test_rpc.import_account,
            "new_password": new_password
        }
        response = requests.post(url=URL, data=json.dumps(invalid_old_password))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 10, json_data['msg'])

        invalid_new_password = {
            "action": "account_password_change",
            "account": Test_rpc.import_account,
            "old_password": new_password
        }
        response = requests.post(url=URL, data=json.dumps(invalid_new_password))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 11, json_data['msg'])

        data = {
            "action": "account_password_change",
            "account": Test_rpc.import_account,
            "old_password": Test_rpc.import_password,
            "new_password": new_password
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json,json_data = try_load_json(response.text)
        self.assertTrue(is_json,response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

        #change password to old one
        data = {
            "action": "account_password_change",
            "account": Test_rpc.import_account,
            "old_password": new_password,
            "new_password": Test_rpc.import_password
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

    def test_account_list(self):
        data = {
            "action": "account_list"
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json,json_data = try_load_json(response.text)
        self.assertTrue(is_json,response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

    def test_account_state_list(self):
        data = {
            "action": "account_state_list",
            "account": Test_rpc.genesis_account,
            "limit": "100",
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

    def test_account_block_list(self):
        data = {
            "action": "account_block_list",
            "account": Test_rpc.import_account,
            "index": 0,
            "limit": 100
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json,json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

    '''
    {
    "code": 0,
    "msg": "OK",
    "signature": "E09CDD795E6959C3B85FDCA0EA56BCFBBC7BE05A0D0AB6B1A0C6DD23FF0AA36F635C70CB731DAC07909A572132128120EBC12862D4BEC2FE70E9A6060F32CA0C"
    }
    '''

    def test_block(self):
        data = {
            "action": "block",
            "hash": Test_rpc.block_hash
        }
        bad_data = {
            "action": "block",
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])
        block_result = json_data['result']
        self.assertTrue(is_hex(block_result['number']))
        self.assertEqual(Test_rpc.block_hash, block_result['hash'])

        response = requests.post(url=URL, data=json.dumps(bad_data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 36, json_data['msg'])

    def test_block_summary(self):
        data = {
            "action": "block_summary",
            "hash": Test_rpc.block_hash
        }
        bad_data = {
            "action": "block",
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])
        self.assertTrue(is_hex(json_data['summary']))

        response = requests.post(url=URL, data=json.dumps(bad_data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 36, json_data['msg'])


    def test_block_state(self):
        data = {
            "action": "block_state",
            "hash": Test_rpc.block_hash
        }
        bad_data = {
            "action": "block_state",
            "hash": ""
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])
        self.assertTrue("content" in json_data["block_state"])

        response = requests.post(url=URL, data=json.dumps(bad_data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])
        self.assertEqual(None, json_data["block_state"])

    def test_block_states(self):
        data = {
            "action": "block_states",
            "hashes": [
                Test_rpc.block_hash,
                Test_rpc.block_hash,
            ]
        }
        bad_data = {
            "action": "block_states",
            "hashes": [
                Test_rpc.block_hash,
                "B222C88AB9729B4DEF3F5E12962DB12A2FA80C9B50A4003CD67CE024428DAC61"
            ]
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json,response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])
        self.assertEqual(len(data['hashes']), len(json_data['block_states']))

        response = requests.post(url=URL, data=json.dumps(bad_data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 37, json_data['msg'])

    def test_block_traces(self):
        data = {
            "action": "block_traces",
            "hash": Test_rpc.block_hash
        }
        bad_data = {
            "action": "block_traces",
            "hash": "sdfksdfjlskdjf"
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json,json_data = try_load_json(response.text)
        self.assertTrue(is_json,response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

        response = requests.post(url=URL, data=json.dumps(bad_data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

    def test_stable_blocks(self):
        data = {
            "action": "stable_blocks",
            "limit": "100",
            "index": "0"
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json,json_data = try_load_json(response.text)
        self.assertTrue(is_json,response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

    def test_status(self):
        data = {
            "action": "status"
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

    '''
    {
    "code": 0,
    "msg": "OK",
    "witness_list": [
        "0x1144B522F45265C2DFDBAEE8E324719E63A1694C",
    ]
    }
    '''
    def test_witness_list(self):
        data = {
            "action": "witness_list"
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json,response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])
        json_witness_list = json_data['witness_list']
        self.assertTrue(len(json_witness_list) > 0, json_witness_list)
        for i in json_data['witness_list']:
            self.assertTrue(is_account(i), json_witness_list)


    '''
    {
    "code": 0,
    "msg": "OK"
    }
    '''
    def test_account_remove(self):
        data = {
            "action": "account_remove",
            "account": Test_rpc.import_account,
            "password": Test_rpc.import_password
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

    '''
    {
    "code": 0,
    "msg": "OK",
    "version": "0.9.7",
    "rpc_version": "1",
    "store_version": "4"
    }
    '''
    def test_version(self):
        data = {
            "action": "version"
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json,response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

        json_version = json_data['version']
        print(type(json_version))
        self.assertTrue(is_str(json_version), json_version)

        json_rpc_version = json_data['rpc_version']
        self.assertTrue(is_str(json_rpc_version), json_rpc_version)

        json_store_version = json_data['store_version']
        self.assertTrue(is_str(json_store_version), json_store_version)

    def test_peers(self):
        data = {
            "action": "peers"
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

    def test_nodes(self):
        data = {
            "action": "nodes"
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

    def test_logs(self):
        data = {
            "action": "logs",
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

    def test_debug_storage_range_at(self):
        data = {
            "action": "debug_storage_range_at",
            "hash": Test_rpc.block_hash,
            "account": Test_rpc.genesis_account,
            "begin": "0000000000000000000000000000000000000000000000000000000000000000",
            "max_results": "100"
        }

        bad_data = {
            "action": "debug_storage_range_at",
            "hash": "9E11690B3B1CB015646ECC549A746B25E08D791E32D38363A905F2A3315C2CC1",
            "account": "mcp4AMgTgyCdzjJCayjyELXgiSqQHiyCXjmCKN174Tiku5jUdGxp5",
            "begin": "0000000000000000000000000000000000000000000000000000000000000000",
            "max_results": 100
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

        response = requests.post(url=URL, data=json.dumps(bad_data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 1, json_data['msg'])

    def test_eth_blockNumber(self):
        data = {
            "method": "eth_blockNumber",
            "params": [],
            "id": 1,
            "jsonrpc": "2.0",
        }
        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_hex(json_data['result']))
        print(json_data)
        print("\n")

    def test_eth_getTransactionCount(self):
        data = {
            "method": "eth_getTransactionCount",
            "params": [Test_rpc.genesis_account],
            "id": 1,
            "jsonrpc": "2.0",
        }
        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_hex(json_data['result']))
        print(json_data)
        print("\n")

    def test_eth_chainId(self):
        data = {
            "method": "eth_chainId",
            "params": [],
            "id": 1,
            "jsonrpc": "2.0",
        }
        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertEqual('0x3cb', json_data['result'])  #Testnet ID
        print(json_data)
        print("\n")

    def test_eth_gasPrice(self):
        data = {
            "method": "eth_gasPrice",
            "params": [],
            "id": 1,
            "jsonrpc": "2.0",
        }
        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_hex(json_data['result']))
        print(json_data)
        print("\n")

    def test_eth_estimateGas(self):
        data = {
            "method": "eth_estimateGas",
            "params": [{
                "from": Test_rpc.import_account,
                "gasPrice": "0x10000",
                "data": "0x608060405234801561001057600080fd5b50336000806101000a81548173ffffffffffffffffffffffffffffffffffffffff021916908373ffffffffffffffffffffffffffffffffffffffff16021790555061019c806100606000396000f3fe608060405234801561001057600080fd5b50600436106100415760003560e01c8063445df0ac146100465780638da5cb5b14610064578063fdacd576146100ae575b600080fd5b61004e6100dc565b6040518082815260200191505060405180910390f35b61006c6100e2565b604051808273ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200191505060405180910390f35b6100da600480360360208110156100c457600080fd5b8101908080359060200190929190505050610107565b005b60015481565b6000809054906101000a900473ffffffffffffffffffffffffffffffffffffffff1681565b6000809054906101000a900473ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff163373ffffffffffffffffffffffffffffffffffffffff16141561016457806001819055505b5056fea265627a7a72315820640b8f7dab5237b9f182a064e48817bd2919b73dd07871a4fb27fef8b2092b0264736f6c63430005100032"
            }],
            "id": 1,
            "jsonrpc": "2.0",
        }
        bad_data = {
            "method": "eth_estimateGas",
            "params": [[{
                "from": Test_rpc.import_account,
                "gasPrice": "0x10000",
                "data": "0x608060405234801561001057600080fd5b50336000806101000a81548173ffffffffffffffffffffffffffffffffffffffff021916908373ffffffffffffffffffffffffffffffffffffffff16021790555061019c806100606000396000f3fe608060405234801561001057600080fd5b50600436106100415760003560e01c8063445df0ac146100465780638da5cb5b14610064578063fdacd576146100ae575b600080fd5b61004e6100dc565b6040518082815260200191505060405180910390f35b61006c6100e2565b604051808273ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff16815260200191505060405180910390f35b6100da600480360360208110156100c457600080fd5b8101908080359060200190929190505050610107565b005b60015481565b6000809054906101000a900473ffffffffffffffffffffffffffffffffffffffff1681565b6000809054906101000a900473ffffffffffffffffffffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff163373ffffffffffffffffffffffffffffffffffffffff16141561016457806001819055505b5056fea265627a7a72315820640b8f7dab5237b9f182a064e48817bd2919b73dd07871a4fb27fef8b2092b0264736f6c63430005100032"
            }]],
            "id": 1,
            "jsonrpc": "2.0",
        }
        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_hex(json_data['result']))
        print(json_data)
        print("\n")

        response = requests.post(url=URL, data=json.dumps(bad_data))
        is_json, json_data = try_load_json(response.text)
        self.assertEqual(json_data['error']['code'],  -32602, json_data['error']['message'])
        print(json_data)
        print("\n")

    def test_eth_getBlockByNumber(self):
        data = {
            "method": "eth_getBlockByNumber",
            "params": ["latest", False],
            "id": 1,
            "jsonrpc": "2.0",
        }
        data_trans = {
            "method": "eth_getBlockByNumber",
            "params": ["5", True],
            "id": 1,
            "jsonrpc": "2.0",
        }
        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_hex(json_data['result']['number']))
        print(json_data)
        print("\n")

        response = requests.post(url=URL, data=json.dumps(data_trans))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue('0x5', json_data['result']['number'])
        self.assertTrue(is_hex(json_data['result']['parentHash']))
        print(json_data)
        print("\n")

    def test_eth_getBlockTransactionCountByHash(self):
        data = {
            "method": "eth_getBlockTransactionCountByHash",
            "params": [Test_rpc.block_hash],
            "id": 1,
            "jsonrpc": "2.0",
        }
        bad_data = {
            "method": "eth_getBlockTransactionCountByHash",
            "params": ["0x3sfjkwejr"],
            "id": 1,
            "jsonrpc": "2.0",
        }
        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_hex(json_data['result']))
        print(json_data)
        print("\n")

        response = requests.post(url=URL, data=json.dumps(bad_data))
        is_json, json_data = try_load_json(response.text)
        self.assertEqual(json_data['result'], None)
        print(json_data)
        print("\n")

    def test_eth_getBlockTransactionCountByNumber(self):
        data_1 = {
            "method": "eth_getBlockTransactionCountByNumber",
            "params": ["latest"],
            "id": 1,
            "jsonrpc": "2.0",
        }
        data_2 = {
            "method": "eth_getBlockTransactionCountByNumber",
            "params": ["earliest"],
            "id": 1,
            "jsonrpc": "2.0",
        }
        data_3 = {
            "method": "eth_getBlockTransactionCountByNumber",
            "params": ["1"],
            "id": 1,
            "jsonrpc": "2.0",
        }

        bad_data = {
            "method": "eth_getBlockTransactionCountByNumber",
            "params": ["dfsdf"],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(data_1))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_hex(json_data['result']))
        print(json_data)
        print("\n")

        response = requests.post(url=URL, data=json.dumps(data_2))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_hex(json_data['result']))
        print(json_data)
        print("\n")

        response = requests.post(url=URL, data=json.dumps(data_3))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_hex(json_data['result']))
        print(json_data)
        print("\n")

        response = requests.post(url=URL, data=json.dumps(bad_data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_hex(json_data['result']))
        print(json_data)
        print("\n")

    '''
    {
      "jsonrpc": "2.0",
      "id": 1,
      "result": "0x3f"
    }
    '''
    def test_eth_protocolVersion(self):
        data = {
            "method": "eth_protocolVersion",
            "params": [],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_str(json_data['result']))
        print(json_data)
        print("\n")

    '''
        {'id': 1, 'jsonrpc': '2.0', 'result': {'startingBlock': '0x19', 'currentBlock': '0x1a', 'highestBlock': '0x19'}}
    '''

    def test_eth_syncing(self):
        data = {
            "method": "eth_syncing",
            "params": [],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_hex(json_data['result']['startingBlock']))
        print(json_data)
        print("\n")

    def test_eth_getLogs(self):
        data = {
            "method": "eth_getLogs",
            "params": [{}],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        print(json_data)
        print("\n")

    '''
    {
        "jsonrpc": "2.0",
        "id": 1,
        "result": "0x60606040..."
    }
    '''
    def test_eth_getCode(self):
        data = {
            "method": "eth_getCode",
            "params": ["0xdd2b08c181d4077e2530a3e41b7271c5d4936ebd","latest"],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_hex(json_data['result']))
        print(json_data)
        print("\n")

    '''
    {
        "jsonrpc": "2.0",
        "id": 1,
        "result": "0x0000000000000000000000000000000000000000000000000000000000000000"
    }
    '''
    def test_eth_getStorageAt(self):
        data = {
            "method": "eth_getStorageAt",
            "params": ["0xdd2b08c181d4077e2530a3e41b7271c5d4936ebd", "latest"],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_hex(json_data['result']))
        print(json_data)
        print("\n")

    '''
    {
      "jsonrpc": "2.0",
      "id": 1,
      "result": "1"
    }
    '''
    def test_net_version(self):
        data = {
            "method": "net_version",
            "params": [],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_hex(json_data['result']))
        print(json_data)
        print("\n")

    '''
    {
      "jsonrpc": "2.0",
      "id": 1,
      "result": true
    }
    '''
    def test_net_listening(self):
        data = {
            "method": "net_listening",
            "params": [],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_boolean(json_data['result']))
        print(json_data)
        print("\n")

    '''
    {
      "jsonrpc": "2.0",
      "id": 1,
      "result": "0x64"
    }
    '''
    def test_net_peerCount(self):
        data = {
            "method": "net_peerCount",
            "params": [],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_hex(json_data['result']))
        print(json_data)
        print("\n")

    '''
    {
        "jsonrpc": "2.0",
        "id": 1,
        "result": "Geth/v1.8.15-omnibus-255989da/linux-amd64/go1.10.1"
    }
    '''
    def test_web3_clientVersion(self):
        data = {
            "method": "web3_clientVersion",
            "params": [],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_str(json_data['result']))
        print(json_data)
        print("\n")

    '''
    {
      "id": 1,
      "jsonrpc": "2.0",
      "result": "0x47173285a8d7341e5e972fc677286384f802f8ef42a5ec5f03bbfa254cb01fad"
    }
    '''
    def test_web3_sha3(self):
        data = {
            "method": "web3_sha3",
            "params": ["0x68656c6c6f20776f726c64"],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(json_data['result'], "0x47173285a8d7341e5e972fc677286384f802f8ef42a5ec5f03bbfa254cb01fad")
        print(json_data)
        print("\n")

    '''
    {
      "jsonrpc": "2.0",
      "id": 1,
      "result": {
        "blockHash": "0xb3b20624f8f0f86eb50dd04688409e5cea4bd02d700bf6e79e9384d47d6a5a35",
        "blockNumber": "0x5bad55",
        "from": "0x398137383b3d25c92898c656696e41950e47316b",
        "gas": "0x1d45e",
        "gasPrice": "0xfa56ea00",
        "hash": "0xbb3a336e3f823ec18197f1e13ee875700f08f03e2cab75f0d0b118dabb44cba0",
        "input": "0xf7d8c88300000000000000000000000000000000000000000000000000000000000cee6100000000000000000000000000000000000000000000000000000000000ac3e1",
        "nonce": "0x18",
        "r": "0x2a378831cf81d99a3f06a18ae1b6ca366817ab4d88a70053c41d7a8f0368e031",
        "s": "0x450d831a05b6e418724436c05c155e0a1b7b921015d0fbc2f667aed709ac4fb5",
        "to": "0x06012c8cf97bead5deae237070f9587f8e7a266d",
        "transactionIndex": "0x11",
        "type": "0x0",
        "v": "0x25",
        "value": "0x1c6bf526340000"
      }
    }
    '''
    def test_eth_getTransactionByHash(self):
        data = {
            "method": "eth_getTransactionByHash",
            "params": [Test_rpc.transaction_hash],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertEqual(json_data['result']['hash'], Test_rpc.transaction_hash)
        print(json_data)
        print("\n")

        bad_data = {
            "method": "eth_getTransactionByHash",
            "params": ["dkfiuweirksdf"],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(bad_data))
        is_json, json_data = try_load_json(response.text)
        self.assertEqual(json_data['result'], None)
        print(json_data)
        print("\n")

    '''
    {
      "jsonrpc": "2.0",
      "id": 1,
      "result": {
        "blockHash": "0xb3b20624f8f0f86eb50dd04688409e5cea4bd02d700bf6e79e9384d47d6a5a35",
        "blockNumber": "0x5bad55",
        "from": "0xfbb1b73c4f0bda4f67dca266ce6ef42f520fbb98",
        "gas": "0x249f0",
        "gasPrice": "0x174876e800",
        "hash": "0x8784d99762bccd03b2086eabccee0d77f14d05463281e121a62abfebcf0d2d5f",
        "input": "0x6ea056a9000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000bd8d7fa6f8cc00",
        "nonce": "0x5e4724",
        "r": "0xd1556332df97e3bd911068651cfad6f975a30381f4ff3a55df7ab3512c78b9ec",
        "s": "0x66b51cbb10cd1b2a09aaff137d9f6d4255bf73cb7702b666ebd5af502ffa4410",
        "to": "0x4b9c25ca0224aef6a7522cabdbc3b2e125b7ca50",
        "transactionIndex": "0x0",
        "type": "0x0",
        "v": "0x25",
        "value": "0x0"
      }
    }
    '''
    def test_eth_getTransactionByBlockHashAndIndex(self):
        data = {
            "method": "eth_getTransactionByBlockHashAndIndex",
            "params": [Test_rpc.block_hash, "0x0"],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertEqual(json_data['result']['blockHash'], Test_rpc.block_hash)
        print(json_data)
        print("\n")

        bad_data = {
            "method": "eth_getTransactionByBlockHashAndIndex",
            "params": [Test_rpc.transaction_hash, "0x0"],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(bad_data))
        is_json, json_data = try_load_json(response.text)
        self.assertEqual(json_data['result'], None)
        print(json_data)
        print("\n")

    '''
    {
      "jsonrpc": "2.0",
      "id": 1,
      "result": {
        "blockHash": "0xb3b20624f8f0f86eb50dd04688409e5cea4bd02d700bf6e79e9384d47d6a5a35",
        "blockNumber": "0x5bad55",
        "from": "0xfbb1b73c4f0bda4f67dca266ce6ef42f520fbb98",
        "gas": "0x249f0",
        "gasPrice": "0x174876e800",
        "hash": "0x8784d99762bccd03b2086eabccee0d77f14d05463281e121a62abfebcf0d2d5f",
        "input": "0x6ea056a9000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000bd8d7fa6f8cc00",
        "nonce": "0x5e4724",
        "r": "0xd1556332df97e3bd911068651cfad6f975a30381f4ff3a55df7ab3512c78b9ec",
        "s": "0x66b51cbb10cd1b2a09aaff137d9f6d4255bf73cb7702b666ebd5af502ffa4410",
        "to": "0x4b9c25ca0224aef6a7522cabdbc3b2e125b7ca50",
        "transactionIndex": "0x0",
        "type": "0x0",
        "v": "0x25",
        "value": "0x0"
      }
    }
    '''
    def test_eth_getTransactionByBlockNumberAndIndex(self):
        data = {
            "method": "eth_getTransactionByBlockNumberAndIndex",
            "params": [Test_rpc.block_number, "0x0"],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertEqual(json_data['result']['blockNumber'], Test_rpc.block_number)
        print(json_data)
        print("\n")

        bad_data = {
            "method": "eth_getTransactionByBlockNumberAndIndex",
            "params": ["0x5BAD55", "0x0"],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(bad_data))
        is_json, json_data = try_load_json(response.text)
        self.assertEqual(json_data['result'], None)
        print(json_data)
        print("\n")

    '''
    {
      "jsonrpc": "2.0",
      "id": 1,
      "result": {
        "blockHash": "0xb3b20624f8f0f86eb50dd04688409e5cea4bd02d700bf6e79e9384d47d6a5a35",
        "blockNumber": "0x5bad55",
        "contractAddress": null,
        "cumulativeGasUsed": "0xb90b0",
        "effectiveGasPrice":"0x746a528800",
        "from": "0x398137383b3d25c92898c656696e41950e47316b",
        "gasUsed": "0x1383f",
        "logs": [
          {
            "address": "0x06012c8cf97bead5deae237070f9587f8e7a266d",
            "blockHash": "0xb3b20624f8f0f86eb50dd04688409e5cea4bd02d700bf6e79e9384d47d6a5a35",
            "blockNumber": "0x5bad55",
            "data": "0x000000000000000000000000398137383b3d25c92898c656696e41950e47316b00000000000000000000000000000000000000000000000000000000000cee6100000000000000000000000000000000000000000000000000000000000ac3e100000000000000000000000000000000000000000000000000000000005baf35",
            "logIndex": "0x6",
            "removed": false,
            "topics": [
              "0x241ea03ca20251805084d27d4440371c34a0b85ff108f6bb5611248f73818b80"
            ],
            "transactionHash": "0xbb3a336e3f823ec18197f1e13ee875700f08f03e2cab75f0d0b118dabb44cba0",
            "transactionIndex": "0x11"
          }
        ],
        "logsBloom": "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000080000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000800000000000000000000000000000000000000000000000000000000000000080000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000800000200000000000000000000000000000",
        "status": "0x1",
        "to": "0x06012c8cf97bead5deae237070f9587f8e7a266d",
        "transactionHash": "0xbb3a336e3f823ec18197f1e13ee875700f08f03e2cab75f0d0b118dabb44cba0",
        "transactionIndex": "0x11",
        "type": "0x0"
      }
    }
    '''
    def test_eth_getTransactionReceipt(self):
        data = {
            "method": "eth_getTransactionReceipt",
            "params": [Test_rpc.transaction_hash],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertEqual(json_data['result']['transactionHash'], Test_rpc.transaction_hash)
        print(json_data)
        print("\n")

        bad_data = {
            "method": "eth_getTransactionReceipt",
            "params": [Test_rpc.block_hash],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(bad_data))
        is_json, json_data = try_load_json(response.text)
        self.assertEqual(json_data['result'], None)
        print(json_data)
        print("\n")

    '''
    {
        "jsonrpc": "2.0",
        "id": 1,
        "result": "0x2fe84e3113d7b"
    }
    '''
    def test_eth_getBalance(self):
        data = {
            "method": "eth_getBalance",
            "params": [Test_rpc.genesis_account],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_hex(json_data['result']))
        print(json_data)
        print("\n")

        bad_data = {
            "method": "eth_getBalance",
            "params": ["qwekqjwkejk"],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(bad_data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_hex(json_data['result']))
        print(json_data)
        print("\n")

    '''
    {
        "jsonrpc": "2.0",
        "id": 1,
        "result": {
            "difficulty": "0xbfabcdbd93dda",
            "extraData": "0x737061726b706f6f6c2d636e2d6e6f64652d3132",
            "gasLimit": "0x79f39e",
            "gasUsed": "0x79ccd3",
            "hash": "0xb3b20624f8f0f86eb50dd04688409e5cea4bd02d700bf6e79e9384d47d6a5a35",
            "logsBloom": "0x4848112002a2020aaa0812180045840210020005281600c80104264300080008000491220144461026015300100000128005018401002090a824a4150015410020140400d808440106689b29d0280b1005200007480ca950b15b010908814e01911000054202a020b05880b914642a0000300003010044044082075290283516be82504082003008c4d8d14462a8800c2990c88002a030140180036c220205201860402001014040180002006860810ec0a1100a14144148408118608200060461821802c081000042d0810104a8004510020211c088200420822a082040e10104c00d010064004c122692020c408a1aa2348020445403814002c800888208b1",
            "miner": "0x5a0b54d5dc17e0aadc383d2db43b0a0d3e029c4c",
            "mixHash": "0x3d1fdd16f15aeab72e7db1013b9f034ee33641d92f71c0736beab4e67d34c7a7",
            "nonce": "0x4db7a1c01d8a8072",
            "number": "0x5bad55",
            "parentHash": "0x61a8ad530a8a43e3583f8ec163f773ad370329b2375d66433eb82f005e1d6202",
            "receiptsRoot": "0x5eced534b3d84d3d732ddbc714f5fd51d98a941b28182b6efe6df3a0fe90004b",
            "sha3Uncles": "0x8a562e7634774d3e3a36698ac4915e37fc84a2cd0044cb84fa5d80263d2af4f6",
            "size": "0x41c7",
            "stateRoot": "0xf5208fffa2ba5a3f3a2f64ebd5ca3d098978bedd75f335f56b705d8715ee2305",
            "timestamp": "0x5b541449",
            "totalDifficulty": "0x12ac11391a2f3872fcd",
            "transactions": [
                "0x8784d99762bccd03b2086eabccee0d77f14d05463281e121a62abfebcf0d2d5f",
                "0x311be6a9b58748717ac0f70eb801d29973661aaf1365960d159e4ec4f4aa2d7f",
                "0xe42b0256058b7cad8a14b136a0364acda0b4c36f5b02dea7e69bfd82cef252a2",
                "0x4eb05376055c6456ed883fc843bc43df1dcf739c321ba431d518aecd7f98ca11",
                "0x994dd9e72b212b7dc5fd0466ab75adf7d391cf4f206a65b7ad2a1fd032bb06d7",
                "0xf6feecbb9ab0ac58591a4bc287059b1133089c499517e91a274e6a1f5e7dce53",
                "0x7e537d687a5525259480440c6ea2e1a8469cd98906eaff8597f3d2a44422ff97",
                "0xa762220e92bed6d77a2c19ffc60dad77d71bd5028c5230c896ab4b9552a39b50",
                "0xf1fa677edda7e5add8e794732c7554cd5459a5c12781dc71de73c7937dfb2775",
                "0x3220af8e317fde6dac80b1199f9ceeafe60ada4974a7e04a75fbce1ac4cb46c3",
                "0x5566528978250828168f0d30bcc8a3689d129c75d820d604f7eb84c25b34ec81",
                "0x646c98e323a05862778f0c9063a989b6aefd94f28842a3a09d2edb37a050717d",
                "0xe951ea55764f7e8e0720f7042dd1db67525965302ed974a0c8e3b769bc1818e3",
                "0x7ecf2528b7df3831712501f5c60ef156bf5fcac9912199e0a64afcb963ea91ca",
                "0xc43b89783f68b2844918ea515cc146c006e5f162c9be9aedf5e7a6ae1f32e164",
                "0xd74503ede63d6fd41367796433aa14439902e8f57293a0583e19aa6ebf3f128e",
                "0x021e5b7d3ddac97b4c6cb9c3f333766a533c1ed9fbcfb8b2515c38ecd0c53f89",
                "0xbb3a336e3f823ec18197f1e13ee875700f08f03e2cab75f0d0b118dabb44cba0",
                "0x25f65866dba34783200c25fb1c120b36326c9ad3a47e8bc34c3edbc9208f1378",
                "0x5336f5c4132ef00e8b469ecfd4ee0d6800f6bd60aefb1c62232cbce81c085ae2",
                "0xb87410cfe0a75c004f7637736b3de1e8f4e08e9e2b05ab963622a40a5505664d",
                "0x990857a27ec7cfd6dfd88015173adf81959b5abaff6eefbe8e9df6b0f40f2711",
                "0x3563ccb5734b7b5015122a20b558723afe992ff1109a04b57e02f26edd5a6a38",
                "0xd7885d9412cc494fbe680b016bf7402b633c34c66833b35cad59af2a4aff4f0b",
                "0x48e60927d6fb9ae76f69a6400490b5ffcb2f9da3105fad6c61f21256ef0c217c",
                "0x9e30af26ff3836c4b55af62ba134bc55db662cf1d396cca437d12a8195bfcbe4",
                "0x2476eeede4764c6871f50f3235ebeb9a56d33b41bc3bb1ce3c18c5d710a0609c",
                "0x1cd3520fbb1eb6f2f6f257ab7c3cba957806b0b87182baedb4f81c62868064c1",
                "0x78ae3aee0ff16d8ea4f394b7b80021804e1d9f35cdbb9c6189bb6cbf58bc52c4",
                "0xfcc75bad728b8d302ba0674ebe3122fc50e3b78fe4948ddfc0d37ee987e666ca",
                "0xd2175464d72bcc61b2e07aa3aac742b4184480d7a9f6ae5c2ba24d9c9bb9f304",
                "0x42b56b504e59e42a3dc94e740bb4231e6326daaac7a73ef93ee8db7b96ac5d71",
                "0xd42681091641cd2a71f18299e8e206d5659c3076b1c63adc26f5b7740e230d2b",
                "0x1202c354f0a00b31adf9e3d895e0c8f3896182bb3ab9fc69d6c21d31a1bf279c",
                "0xa5cea1f6957431caf589a8dbb58c102fb191b39967fbe8d26cecf6f28bb835da",
                "0x2045efeb2f5ea9176690ece680d3fd7ca9e945d0d572d17786810d323628f98c",
                "0xbf55d13976616a23114b724b14049eaaf91db3f1950320b5306006a6b648b24f",
                "0x9e5c5ea885eb1d6b1b3ffcf703e3381b7681f7420f35408d30ba93ec0cdf0792",
                "0x6f1a61dc4306ca5e976a1706afe1f32279548df98e0373c5fee0ea189ddb77a0",
                "0xc5c16b30c22ee4f90c3a2de70554f7975eb476592ff13c61986d760da6cf7f9d",
                "0xb09de28497227c0537df0a78797fa00407dcd04a4f90d9de602484b61f7bf169",
                "0x1bfea966fa7772a26b4b2c8add15ceedcb70a903618f5d4603d69f52b9954025",
                "0xe58be9c0e3cedd4444c76d1adc098ba40cbe21ef886b2bfc2edb6ed96ba8d966",
                "0x3a29096f712ccdafd56e9a3c635d4fe2e6224ac3666d466c21da66c8829bbfd6",
                "0x31feab77d7c1c87eb79af54193400c8edad16645e1ea5fcc10f2eaec51fe3992",
                "0x4e0278fce62dca8e23cfae6a020fcd3b2facc03244d54b964bbde424f902ffe1",
                "0x300239a64a50ad0e646c232f85cfa4f3d3ed30090cd574329c782d95c2b42532",
                "0x41755f354b06b4b8a452db1cc9b5c810c75b1bbe236603cbc0950c3c81b80c51",
                "0x1e3fbeffc326f1ffd8559c6024c12557e6014bc02c12d65dbc1baa4e1aed94b7",
                "0x4a459a32cf68e9b7697a3a656432b340d6d27c3d4a513e6cce770d63df99839a",
                "0x3ef484913d185de728c787a1053ec1444ec1c7a5827eecba521d3b406b088a89",
                "0x43afa584c21f27a2747a8397b00d3ec4b460d929b61b510d017f01037a3ded3f",
                "0x44e6a37a6c1d8696fa0537385b9d1bb535b2b3309b5482209e95b5b6c58fc8da",
                "0x2a8bca48147955efcfd697f1a97304ae4cc467a7778741c2c47e516610f0a876",
                "0x4c6bd64c8974f8b949cfe265da1c1bb997e3c886f024b38c99d170acc70b83df",
                "0x103f0cca1ae13600c5be5b217e92430a72b0471d05e283c105f5d0df36438b2a",
                "0x00a06bf6fbd07b3a89ef9031a2108c8fa31b467b33a6edcd6eb3687c158743cf",
                "0x0175496d8265dedd693cf88884626c33b699ebcf4f2110e4c7fb7603c53215b2",
                "0x11fb433ab551b33f30d00a34396835fab72e316e81d1e0afcbc92e79801f30c4",
                "0x060dc4541fd534d107f6e49b96d84f5ec6dbe4eb714890e800bd02399a6bfb7f",
                "0x01956de9f96f9a268c6524fffb9919d7fa3de7a4c25d53c2ccc43d0cb022a7ff",
                "0x15057378f2d223829269ec0f31ba4bb03146134220d34eb8eb7c403aa4a2e569",
                "0x16ea0218d72b5e3f69d0ae4daa8085150f5f7e69ee22a3b054744e35e2082879",
                "0x0baf4e8ff92058c1cac3b95c237edb4d2c12ad41d210356c209f1e0bf0d2d12a",
                "0x1a8ac77aff614caeca16a5a3a0931375a5a4fbe0ef1e15d6d15bf6f8e3c60f4f",
                "0xdb899136f41a3d4710907345b09d241490776383271e6b9887499fd05b80fcd4",
                "0x1007e17b1120d37fb930f953d8a3440ca11b8fd84470eb107c8b4a402a9813fd",
                "0x0910324706ffeebf8aa25ca0784636518bf67e5d173c22438a64dd43d5f4aa2a",
                "0x028f2bee56aee7005abcb2258d6d9f0f078a85a65c3d669aca40564ef4bd7f94",
                "0x14adac9bc94cde3166f4b7d42e8862a745483c708e51afbe89ecd6532acc532e",
                "0x54bed12ccad43523ba8527d1b99f5fa04a55b3a7724cfff2e0a21ec90b08590e",
                "0xcdf05df923f6e418505750069d6486276b15fcc3cd2f42a7044c642d19a86d51",
                "0x0c66977ed87db75074cb2bea66b254af3b20bb3315e8095290ceb1260b1b7449",
                "0x22626e2678da34b505b233ef08fc91ea79c5006dff00e33a442fa51a11e34c25",
                "0xe2989560000a1fc7c434c5e9c4bba82e1501bf435292ac25acc3cb182c1c2cd0",
                "0x348cfc85c58b7f3b2e8bdaa517dc8e3c5f8fb41e3ba235f28892b46bc3484756",
                "0x4ac009cebc1f2416b9e39bcc5b41cd53b1a9239e8f6c0ab043b8830ef1ffc563",
                "0xf2a96682362b9ffe9a77190bcbc47937743b6e1da2c56257f9b562f15bbd3cfa",
                "0xf1cd627c97746bc75727c2f0efa2d0dc66cca1b36d8e45d897e18a9b19af2f60",
                "0x241d89f7888fbcfadfd415ee967882fec6fdd67c07ca8a00f2ca4c910a84c7dd"
            ],
            "transactionsRoot": "0xf98631e290e88f58a46b7032f025969039aa9b5696498efc76baf436fa69b262",
            "uncles": [
                "0x824cce7c7c2ec6874b9fa9a9a898eb5f27cbaf3991dfa81084c3af60d1db618c"
            ]
        }
    }
    '''
    def test_eth_getBlockByHash(self):
        data = {
            "method": "eth_getBlockByHash",
            "params": [Test_rpc.block_hash],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertEqual(json_data['result']['hash'], Test_rpc.block_hash)
        print(json_data)
        print("\n")

        bad_data = {
            "method": "eth_getBlockByHash",
            "params": [Test_rpc.transaction_hash],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(bad_data))
        is_json, json_data = try_load_json(response.text)
        self.assertEqual(json_data['result'], None)
        print(json_data)
        print("\n")

    '''
    {
      "jsonrpc": "2.0",
      "id": 1,
      "result": []
    }
    '''
    def test_eth_accounts(self):
        data = {
            "method": "eth_accounts",
            "params": [],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(json_data['result'] is not None)
        print(json_data)
        print("\n")

    '''
    {
      "id": 1,
      "jsonrpc": "2.0",
      "result": "0xe670ec64341771606e55d6b4ca35a1a6b75ee3d5145a99d05921026d1527331"
    }
    '''
    def test_eth_sendTransaction(self):
        data = {
            "method": "eth_sendTransaction",
            "params": [{
                "from": Test_rpc.genesis_account,
                "to": Test_rpc.import_account,
                "gas": "0x76c0",
                "gasPrice": "0x9184e72a000",
                "value": "0x9184e72a",
                "data": ""
            }],
            "id": 1,
            "jsonrpc": "2.0",
        }

        # Should be failed before unlock
        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(json_data['result'] is None)
        self.assertEqual(json_data['error']['code'], -32602, json_data['error']['message'])
        print(json_data)
        print("\n")

        # unlock account before send
        unlock_data = {
            "action": "account_unlock",
            "account": Test_rpc.genesis_account,
            "password": Test_rpc.import_password
        }
        response = requests.post(url=URL, data=json.dumps(unlock_data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

        # resend after unlock
        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_hex(json_data['result']))
        print(json_data)
        print("\n")

        # lock again after sign
        lock_data = {
            "action": "account_lock",
            "account": Test_rpc.genesis_account,
            "password": Test_rpc.import_password
        }
        response = requests.post(url=URL, data=json.dumps(lock_data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

    def test_eth_sign(self):
        data = {
            "method": "eth_sign",
            "params": [
                Test_rpc.genesis_account,
                "0x12345678",
            ],
            "id": 1,
            "jsonrpc": "2.0",
        }

        #Should be failed before unlock
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        self.assertTrue(json_data['result'] is None)
        self.assertEqual(json_data['error']['code'], -32602, json_data['error']['message'])

        unlock_data = {
            "action": "account_unlock",
            "account": Test_rpc.genesis_account,
            "password": Test_rpc.import_password
        }
        response = requests.post(url=URL, data=json.dumps(unlock_data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

        # sign after unlock
        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_hex(json_data['result']))
        print(json_data)
        print("\n")

        # lock again after sign
        lock_data = {
            "action": "account_lock",
            "account": Test_rpc.genesis_account,
            "password": Test_rpc.import_password
        }
        response = requests.post(url=URL, data=json.dumps(lock_data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

    def test_eth_signTransaction(self):
        data = {
            "method": "eth_signTransaction",
            "params": [{
                "from": Test_rpc.genesis_account,
                "to": Test_rpc.import_account,
                "gas": "0x76c0",
                "gasPrice": "0x9184e72a000",
                "value": "0x9184e72a",
                "data": "0xd46e8dd67c5d32be8d46e8dd67c5d32be8058bb8eb970870f072445675058bb8eb970870f072445675"
                }
            ],
            "id": 1,
            "jsonrpc": "2.0",
        }
        # Should be failed before unlock
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        self.assertTrue(json_data['result'] is None)
        self.assertEqual(json_data['error']['code'], -32602, json_data['error']['message'])

        unlock_data = {
            "action": "account_unlock",
            "account": Test_rpc.genesis_account,
            "password": Test_rpc.import_password
        }
        response = requests.post(url=URL, data=json.dumps(unlock_data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

        # sign after unlock
        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertEqual(json_data['result']['tx']['from'].lower(), Test_rpc.genesis_account.lower())
        self.assertTrue(is_hex(json_data['result']['raw']))
        print(json_data)
        print("\n")

        send_raw_data = {
            "method": "eth_sendRawTransaction",
            "params": [json_data['result']['raw']],
            "id": 1,
            "jsonrpc": "2.0",
        }
        response = requests.post(url=URL, data=json.dumps(send_raw_data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_hex(json_data['result']))
        print(json_data)
        print("\n")

        # lock again after sign
        lock_data = {
            "action": "account_lock",
            "account": Test_rpc.genesis_account,
        }
        response = requests.post(url=URL, data=json.dumps(lock_data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

    def test_personal_listAccounts(self):
        data = {
            "method": "personal_listAccounts",
            "params": [],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(json_data['result'] is not None)
        print(json_data)
        print("\n")

    def test_personal_importRawKey(self):
        data = {
            "method": "personal_importRawKey",
            "params": ["d79703a37d55fd5afc17fa4bf98047f9c6592559abe107d01fad13f8cdd0cd2a", "12345678"],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertEqual(json_data['result'].lower(), Test_rpc.genesis_account.lower())
        print(json_data)
        print("\n")

    def test_personal_lockAccount(self):
        data = {
            "method": "personal_lockAccount",
            "params": [Test_rpc.genesis_account],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(json_data['result'])
        print(json_data)
        print("\n")

        bad_data = {
            "method": "personal_lockAccount",
            "params": [Test_rpc.import_account],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(bad_data))
        is_json, json_data = try_load_json(response.text)
        self.assertFalse(json_data['result'])
        print(json_data)
        print("\n")

    def test_personal_newAccount(self):
        data = {
            "method": "personal_newAccount",
            "params": [Test_rpc.import_password],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_account(json_data['result']))
        print(json_data)
        print("\n")

        bad_data = {
            "method": "personal_newAccount",
            "params": [""],
            "id": 1,
            "jsonrpc": "2.0",
        }

        response = requests.post(url=URL, data=json.dumps(bad_data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(json_data['result'] is None)
        self.assertEqual(json_data['error']['code'], -32602, json_data['error']['message'])
        print(json_data)
        print("\n")

    def test_personal_sendTransaction(self):
        data = {
            "method": "personal_sendTransaction",
            "params": [{
                "from": Test_rpc.genesis_account,
                "to": Test_rpc.import_account,
                "gas": "0x76c0",
                "gasPrice": "0x9184e72a000",
                "value": "0x9184e72a",
                "data": ""
            }, Test_rpc.import_password],
            "id": 1,
            "jsonrpc": "2.0",
        }

        # Should be failed before unlock
        response = requests.post(url=URL, data=json.dumps(data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_hex(json_data['result']))
        print(json_data)
        print("\n")

    def test_personal_unlockAccount(self):
        unlock_data = {
            "method": "personal_unlockAccount",
            "params": [
                Test_rpc.genesis_account,
                Test_rpc.import_password,
                ],
            "id": 1,
            "jsonrpc": "2.0",
        }
        response = requests.post(url=URL, data=json.dumps(unlock_data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertTrue(json_data['result'])

        lock_data = {
            "method": "personal_lockAccount",
            "params": [Test_rpc.genesis_account],
            "id": 1,
            "jsonrpc": "2.0",
        }
        response = requests.post(url=URL, data=json.dumps(lock_data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(json_data['result'])
        print(json_data)
        print("\n")

    def test_personal_sign(self):
        unlock_data = {
            "method": "personal_unlockAccount",
            "params": [
                Test_rpc.genesis_account,
                Test_rpc.import_password,
            ],
            "id": 1,
            "jsonrpc": "2.0",
        }
        response = requests.post(url=URL, data=json.dumps(unlock_data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertTrue(json_data['result'])

        data = {
            "method": "personal_sign",
            "params": [
                "0xdeadbeaf",
                Test_rpc.genesis_account,
                Test_rpc.import_password,
            ],
            "id": 1,
            "jsonrpc": "2.0",
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertTrue(is_hex(json_data['result']))

        lock_data = {
            "method": "personal_lockAccount",
            "params": [Test_rpc.genesis_account],
            "id": 1,
            "jsonrpc": "2.0",
        }
        response = requests.post(url=URL, data=json.dumps(lock_data))
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(json_data['result'])
        print(json_data)
        print("\n")

    def test_personal_ecRecover(self):
        data = {
            "method": "personal_sign",
            "params": [
                "0xdeadbeaf",
                Test_rpc.genesis_account,
                Test_rpc.import_password,
            ],
            "id": 1,
            "jsonrpc": "2.0",
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)

        cover_data = {
            "method": "personal_ecRecover",
            "params": [
                "0xdeadbeaf",
                json_data['result'],
            ],
            "id": 1,
            "jsonrpc": "2.0",
        }
        response = requests.post(url=URL, data=json.dumps(cover_data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertTrue(is_hex(json_data['result']))
        self.assertEqual(json_data['result'].lower(), Test_rpc.genesis_account.lower())

    def test_epoch_approves(self):
        data = {
            "action": "epoch_approves",
            "epoch": "0"
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

    def test_epoch_approve_receipts(self):
        data = {
            "action": "epoch_approve_receipts",
            "epoch": "0"
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

    def test_epoch_elected_approve_receipts(self):
        data = {
            "action": "epoch_elected_approve_receipts",
            "epoch": "0"
        }
        response = requests.post(url=URL, data=json.dumps(data))
        self.assertEqual(response.status_code, 200)
        is_json, json_data = try_load_json(response.text)
        self.assertTrue(is_json, response.text)
        json_data = json.loads(response.text)
        self.assertEqual(json_data['code'], 0, json_data['msg'])

if __name__ == "__main__":
    suite = unittest.TestSuite()
    suite.addTest(Test_rpc("test_account_import")) # need to check bad_data
    suite.addTest(Test_rpc("test_account_create"))
    suite.addTest(Test_rpc("test_accounts_balances"))
    suite.addTest(Test_rpc("test_account_balance"))
    suite.addTest(Test_rpc("test_eth_call"))
    suite.addTest(Test_rpc("test_account_code"))
    suite.addTest(Test_rpc("test_account_lock"))
    suite.addTest(Test_rpc("test_account_unlock"))
    suite.addTest(Test_rpc("test_account_export"))
    suite.addTest(Test_rpc("test_account_validate"))
    suite.addTest(Test_rpc("test_account_password_change"))
    suite.addTest(Test_rpc("test_account_list"))
    suite.addTest(Test_rpc("test_account_state_list"))
    suite.addTest(Test_rpc("test_account_block_list"))
    suite.addTest(Test_rpc("test_block"))
    suite.addTest(Test_rpc("test_block_summary"))  #summary has issue
    suite.addTest(Test_rpc("test_block_state"))
    suite.addTest(Test_rpc("test_block_states"))
    suite.addTest(Test_rpc("test_block_traces"))
    suite.addTest(Test_rpc("test_stable_blocks"))
    suite.addTest(Test_rpc("test_status"))
    suite.addTest(Test_rpc("test_witness_list"))
    suite.addTest(Test_rpc("test_version"))
    suite.addTest(Test_rpc("test_peers"))
    suite.addTest(Test_rpc("test_nodes"))
    suite.addTest(Test_rpc("test_logs"))
    suite.addTest(Test_rpc("test_debug_storage_range_at"))
    suite.addTest(Test_rpc("test_account_remove"))
    suite.addTest(Test_rpc("test_eth_blockNumber"))
    suite.addTest(Test_rpc("test_eth_getTransactionCount"))
    suite.addTest(Test_rpc("test_eth_chainId"))
    suite.addTest(Test_rpc("test_eth_gasPrice"))
    suite.addTest(Test_rpc("test_eth_estimateGas"))
    suite.addTest(Test_rpc("test_eth_getBlockByNumber"))
    suite.addTest(Test_rpc("test_eth_getBlockTransactionCountByHash"))
    suite.addTest(Test_rpc("test_eth_getBlockTransactionCountByNumber"))
    suite.addTest(Test_rpc("test_eth_protocolVersion"))
    suite.addTest(Test_rpc("test_eth_syncing"))
    suite.addTest(Test_rpc("test_eth_getLogs"))  #needed to check again when full data
    suite.addTest(Test_rpc("test_eth_getCode"))
    suite.addTest(Test_rpc("test_eth_getStorageAt"))  #needed to check on the MCP
    suite.addTest(Test_rpc("test_net_version"))
    suite.addTest(Test_rpc("test_net_listening"))
    suite.addTest(Test_rpc("test_net_peerCount"))
    suite.addTest(Test_rpc("test_web3_clientVersion"))
    suite.addTest(Test_rpc("test_eth_getTransactionByHash"))
    suite.addTest(Test_rpc("test_eth_getTransactionByBlockHashAndIndex"))
    suite.addTest(Test_rpc("test_eth_getTransactionByBlockNumberAndIndex"))
    suite.addTest(Test_rpc("test_eth_getTransactionReceipt"))
    suite.addTest(Test_rpc("test_eth_getBalance"))
    suite.addTest(Test_rpc("test_eth_getBlockByHash"))
    suite.addTest(Test_rpc("test_eth_accounts"))
    suite.addTest(Test_rpc("test_eth_sendTransaction"))
    suite.addTest(Test_rpc("test_personal_listAccounts"))
    suite.addTest(Test_rpc("test_personal_lockAccount"))
    suite.addTest(Test_rpc("test_personal_newAccount"))
    suite.addTest(Test_rpc("test_personal_unlockAccount"))
    suite.addTest(Test_rpc("test_personal_sendTransaction"))
    suite.addTest(Test_rpc("test_personal_sign"))
    suite.addTest(Test_rpc("test_personal_ecRecover"))
    suite.addTest(Test_rpc("test_epoch_approves"))
    suite.addTest(Test_rpc("test_epoch_approve_receipts"))
    suite.addTest(Test_rpc("test_epoch_elected_approve_receipts"))
    suite.addTest(Test_rpc("test_eth_sign"))
    suite.addTest(Test_rpc("test_web3_sha3"))
    suite.addTest(Test_rpc("test_eth_signTransaction"))
    suite.addTest(Test_rpc("test_personal_importRawKey"))

    result = unittest.TextTestRunner(verbosity=3).run(suite)
    if result.wasSuccessful():
        exit(0)
    else:
        exit(1)

