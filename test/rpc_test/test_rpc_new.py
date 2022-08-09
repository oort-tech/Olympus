# -*-encoding: utf-8-*-
from pickle import FALSE
import unittest
import json
import requests
import re
import string
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

class Test_rpc(unittest.TestCase):
	@classmethod
	def setUpClass(cls):
		Test_rpc.genesis_account = "0x1144B522F45265C2DFDBAEE8E324719E63A1694C"
		Test_rpc.import_account = "0xaa98fd8298939186440dd9c32f716a0b0cec376b"
		Test_rpc.import_password = "12345678"
		Test_rpc.import_public_key = "F16C3C1E3775B13C139038740F976E5E549A41D94E502E3DF4BE118CD81D5310459E5FA7F5A95B7DBC935E30BB55D624DF8F767280D1BAF329EC7E5EF96BF137"
		Test_rpc.to_account = "0xC63BE4C25041F761C5E8D9AA73FEFC57E4AA655B"
	
	'''
	{
	"code": 0,
	"msg": "OK",
	}
	'''
	def test_account_import(self):
		data = {
			"action": "account_import",
			"json": "{\"account\":\"0xaa98fd8298939186440dd9c32f716a0b0cec376b\",\"kdf_salt\":\"0a31f8d656aad9917e4d3fb83dd2662b\",\"iv\":\"f3b9f8b305d2aa39d32994c17d05e3e6\",\"ciphertext\":\"61560379be51fdfa998c198db2b685a4b14d225746729d7319c5cbc10ba28f6a\"}"
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
			"account": Test_rpc.import_account
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
			"account": Test_rpc.import_account,
			"password": Test_rpc.import_password
		}

		invalid_password_data = {
			"action": "account_unlock",
			"account": Test_rpc.import_account
		}

		wrong_password_data = {
			"action": "account_unlock",
			"account": Test_rpc.import_account,
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
		response = requests.post(url=URL, data=json.dumps(data))
		self.assertEqual(response.status_code, 200)
		is_json,json_data = try_load_json(response.text)
		self.assertTrue(is_json,response.text)
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
			"account": Test_rpc.import_account
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
		is_json,json_data = try_load_json(response.text)
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
			"hash": "412254AB895FD2E6ADE6F9076CA8297516F2845C989A13AC008CD5D70157AFFB"
		}
		response = requests.post(url=URL, data=json.dumps(data))
		self.assertEqual(response.status_code, 200)
		is_json, json_data = try_load_json(response.text)
		self.assertTrue(is_json, response.text)
		json_data = json.loads(response.text)
		self.assertEqual(json_data['code'], 0, json_data['msg'])

	def test_block_state(self):
		data = {
			"action": "block_state",
			"hash": "412254AB895FD2E6ADE6F9076CA8297516F2845C989A13AC008CD5D70157AFFB"
		}
		response = requests.post(url=URL, data=json.dumps(data))
		self.assertEqual(response.status_code, 200)
		is_json,json_data = try_load_json(response.text)
		self.assertTrue(is_json,response.text)
		json_data = json.loads(response.text)
		self.assertEqual(json_data['code'], 0, json_data['msg'])

	def test_block_states(self):
		data = {
			"action": "block_states",
			"hashes": [
				"412254AB895FD2E6ADE6F9076CA8297516F2845C989A13AC008CD5D70157AFFB",
				"B222C88AB9729B4DEF3F5E12962DB12A2FA80C9B50A4003CD67CE024428DAC61"
			]
		}
		response = requests.post(url=URL, data=json.dumps(data))
		self.assertEqual(response.status_code, 200)
		is_json,json_data = try_load_json(response.text)
		self.assertTrue(is_json,response.text)
		json_data = json.loads(response.text)
		self.assertEqual(json_data['code'], 0, json_data['msg'])

	def test_block_traces(self):
		data = {
			"action": "block_traces",
			"hash": "412254AB895FD2E6ADE6F9076CA8297516F2845C989A13AC008CD5D70157AFFB"
		}
		response = requests.post(url=URL, data=json.dumps(data))
		self.assertEqual(response.status_code, 200)
		is_json,json_data = try_load_json(response.text)
		self.assertTrue(is_json,response.text)
		json_data = json.loads(response.text)
		self.assertEqual(json_data['code'], 0, json_data['msg'])

	def test_stable_blocks(self):
		data = {
			"action": "stable_blocks",
			"limit": 100,
			"index": 15577
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
		is_json,json_data = try_load_json(response.text)
		self.assertTrue(is_json,response.text)
		json_data = json.loads(response.text)
		self.assertEqual(json_data['code'], 0, json_data['msg'])
	
	'''
	{
    "code": 0,
    "msg": "OK",
    "witness_list": [
        "mcp_321JDA7Brgbnm64iY2Xh8yHMEqEgBDutnoTKVLcxW2DJvJLUsS",
        "mcp_32RmC9FsxjgLkgRQ58j3CdLg79cQE3KaY2wAT1QthBTU25vpd3",
        "mcp_3MnXfV9hbmxVPdgfrPqgUiH6N7VbkSEhn5VqBCzBcxzTzkEUxU",
        "mcp_3SrfL6LnPbtyf6sanrgtKs1BTYDN8taacGBVG37LfZVqXvRHbf",
        "mcp_3igvJpdDiV4v5HxEzCifFcUpKvWsk3qWYNrTrbEVQztKbpyW1z",
        "mcp_3tiy2jgoUENkszPjrHjQGfmopqwV5m9BcEh2Grb1zDYgSGnBF7",
        "mcp_47E2jJ9rXVk5GRBcTLQMLQHXqsrnVcV5Kv2CWQJ6dnUaugnvii",
        "mcp_49BvoaSgGnyfPdaHfrSdac74fcxV4cUdysskHSQPQ8XisShN3P",
        "mcp_4HhYojuHanxQ57thkSxwy5necRtDFwiQP7zqngBDZHMjqdPiMS",
        "mcp_4MYTD6Xctkb6fEL8xUZxUwY6eqYB7ReEfB61YFrMHaZxsqLCKd",
        "mcp_4URkteqck9rM8Vo6VzWmvKtMWoSH8vo4A1rADNAFrQHxAR23Tb",
        "mcp_4ZJ8hBdR6dLv4hb1RPCmajdZf7ozkH1sHU18kT7xnXj4mjxxKE",
        "mcp_4aBXjWXyN7WVGqMKH7FgnSoN9oePeEPiZsrtc2AMYyuTRJoNpb",
        "mcp_4iig3fTcXQmz7bT2ztJPrpH8usrqGTN5zmygFqsCJQ4HgiuNvP"
    ]
	}
	'''
	def test_witness_list(self):
		data = {
			"action": "witness_list"
		}
		response = requests.post(url=URL, data=json.dumps(data))
		self.assertEqual(response.status_code, 200)
		is_json,json_data = try_load_json(response.text)
		self.assertTrue(is_json,response.text)
		json_data = json.loads(response.text)
		self.assertEqual(json_data['code'], 0, json_data['msg'])
		json_witness_list = json_data['witness_list']
		self.assertTrue(len(json_witness_list)>0,json_witness_list)
		for i in json_data['witness_list']:
			self.assertTrue(is_account(i),json_witness_list)
		
	
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
		is_json,json_data = try_load_json(response.text)
		self.assertTrue(is_json,response.text)
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
		is_json,json_data = try_load_json(response.text)
		self.assertTrue(is_json,response.text)
		json_data = json.loads(response.text)
		self.assertEqual(json_data['code'], 0, json_data['msg'])
		
		json_version = json_data['version']
		print(type(json_version))
		self.assertTrue(is_str(json_version),json_version)
		
		json_rpc_version = json_data['rpc_version']
		self.assertTrue(is_str(json_rpc_version),json_rpc_version)
		
		json_store_version = json_data['store_version']
		self.assertTrue(is_str(json_store_version),json_store_version)


	def test_eth_blockNumber(self):
		data = {
			"method": "eth_blockNumber",
			"params": [],
			"id": 1,
			"jsonrpc": "2.0",
		}
		response = requests.post(url=URL, data=json.dumps(data))
		is_json,json_data = try_load_json(response.text)

		print(json_data)
		print("\n")

	def test_eth_getTransactionCount(self):
		data = {
			"method": "eth_getTransactionCount",
			"params": [Test_rpc.import_account],
			"id": 1,
			"jsonrpc": "2.0",
		}
		response = requests.post(url=URL, data=json.dumps(data))
		is_json,json_data = try_load_json(response.text)

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
		is_json,json_data = try_load_json(response.text)

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
		is_json,json_data = try_load_json(response.text)

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
		response = requests.post(url=URL, data=json.dumps(data))
		is_json,json_data = try_load_json(response.text)

		print(json_data)
		print("\n")
	
	def test_eth_getBlockByNumber(self):
		data = {
			"method": "eth_getBlockByNumber",
			"params": ["latest", False],
			"id": 1,
			"jsonrpc": "2.0",
		}
		response = requests.post(url=URL, data=json.dumps(data))
		is_json,json_data = try_load_json(response.text)

		print(json_data)
		print("\n")

if __name__ == "__main__":
	suite = unittest.TestSuite()
	# suite.addTest(Test_rpc("test_account_import")) # need to check bad_data
	# suite.addTest(Test_rpc("test_account_create"))
	# suite.addTest(Test_rpc("test_send_block")) is removed from RPC
	# suite.addTest(Test_rpc("test_accounts_balances"))
	# suite.addTest(Test_rpc("test_account_balance"))
	# suite.addTest(Test_rpc("test_eth_call"))
	# suite.addTest(Test_rpc("test_account_code"))
	# suite.addTest(Test_rpc("test_account_lock"))
	# suite.addTest(Test_rpc("test_account_unlock"))
	# suite.addTest(Test_rpc("test_account_export"))
	# suite.addTest(Test_rpc("test_account_validate"))
	# suite.addTest(Test_rpc("test_account_password_change"))
	# suite.addTest(Test_rpc("test_account_list"))
	# suite.addTest(Test_rpc("test_account_block_list"))

	# suite.addTest(Test_rpc("test_block"))
	# suite.addTest(Test_rpc("test_block_state"))
	# suite.addTest(Test_rpc("test_block_states"))
	# suite.addTest(Test_rpc("test_block_traces"))
	# suite.addTest(Test_rpc("test_stable_blocks"))
	# suite.addTest(Test_rpc("test_status"))
	# suite.addTest(Test_rpc("test_witness_list"))
	# suite.addTest(Test_rpc("test_version"))
	# suite.addTest(Test_rpc("test_account_remove"))

	# suite.addTest(Test_rpc("test_eth_blockNumber"))
	# suite.addTest(Test_rpc("test_eth_getTransactionCount"))
	# suite.addTest(Test_rpc("test_eth_chainId"))
	# suite.addTest(Test_rpc("test_eth_gasPrice"))
	# suite.addTest(Test_rpc("test_eth_estimateGas"))
	# suite.addTest(Test_rpc("test_eth_getBlockByNumber"))

	result = unittest.TextTestRunner(verbosity=3).run(suite)
	if result.wasSuccessful():
		exit(0)
	else:
		exit(1)

