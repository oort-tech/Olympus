# -*-encoding: utf-8-*-
import unittest
import json
import requests
import re
URL = "http://127.0.0.1:8765"
#0xC98A676DE3E0C539742E3023F7755C57E331E42F
#Judge account number, mcp_start, remove I, O, l, 0, length equal to 42	
def is_account(str):
	if re.findall(r'0x[0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ]{40,}',str):
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
def is_hex(str,is_lens=None):
	if is_lens is None:
		if re.findall(r"^[A-F0-9]*$",str) and len(str) % 2 == 0:
			return True
		else:
			return False
	else:
		if re.findall(r"^[A-F0-9]{{{0}}}$".format(is_lens),str):
			return True
		else:
			return False
		
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
		return True,json_data
	except ValueError as e:
		return False,None

class Test_rpc(unittest.TestCase):
	@classmethod
	def setUpClass(cls):
		Test_rpc.genesis_account = "0x38369682E93F5C20A4DA3E374805C9584EA3E2C8"
		Test_rpc.import_account = "0x41A358A4900A0A75BD50304A57E89D455966DC0F"
		Test_rpc.import_password = "1234qwer"
		Test_rpc.import_public_key = "F16C3C1E3775B13C139038740F976E5E549A41D94E502E3DF4BE118CD81D5310459E5FA7F5A95B7DBC935E30BB55D624DF8F767280D1BAF329EC7E5EF96BF137"
		Test_rpc.to_account = "0xEED0A750EFDCB1809CB10CDEE8F3A803A384F600"
	
	'''
	{
	"code": 0,
	"msg": "OK",
	"account": "mcp_3dUnMEsuSiUsKGgfft5VDpM2bX9S6T4ppApHRfn1cBmn2znyEv"
	}
	'''
	def test_account_import(self):
		data = {
			"action": "account_import",
			"json": "{\"account\":\"0x41A358A4900A0A75BD50304A57E89D455966DC0F\",\"public_key\":\"F16C3C1E3775B13C139038740F976E5E549A41D94E502E3DF4BE118CD81D5310459E5FA7F5A95B7DBC935E30BB55D624DF8F767280D1BAF329EC7E5EF96BF137\",\"kdf_salt\":\"F36CA7C846C345960B055C6F10DFD7FD\",\"iv\":\"7B4F5CCB4A201302C1CE8CA7979EDA54\",\"ciphertext\":\"09FD6AEDF849A66AB58C15B12F5F9B901A482521AEB0BD160AF4181D9F14B004\"}"
		}
		# data = {
		# 	"action": "account_import",
		# 	"json": "{\"account\":\"0x38369682E93F5C20A4DA3E374805C9584EA3E2C8\",\"public_key\":\"DEC7B77140D9B2AF105D574A18EDB502F938E385371D93176E90D83E570A7A4C66953B05E61330E617E18675CA060E6E81617488876731DFEB472F52703A4DC0\",\"kdf_salt\":\"AC8D68CF817842F70AF83881CCA1FFD6\",\"iv\":\"D32D6D53C5480C1ED6C032439BFBFDCC\",\"ciphertext\":\"8F9C4680CD6818E201455E2EF21C3AC23A6CB2158F21688218723700476CD038\"}"
		# }
		response = requests.post(url=URL, data=json.dumps(data))
		self.assertEqual(response.status_code, 200)
		
		is_json,json_data = try_load_json(response.text)
		self.assertTrue(is_json, response.text)
		
		self.assertEqual(json_data['code'], 0, json_data['msg'])
		self.assertEqual(json_data['msg'], 'OK', json_data['code'])
		json_account = json_data['account']
		self.assertTrue(is_account(json_account),json_account)

		print(json_data)
		print("\n")
	
	'''
	{
	"code": 0,
	"msg": "OK",
	"account": "mcp_4M943gNHekWpfTmRFJHUYTYV65gnkjN5zAjqTeRTbnNCXfeJrw"
	}
	'''
	def test_account_create(self):
		data = {
			"action": "account_create",
			"password": Test_rpc.import_password
		}
		response = requests.post(url=URL, data=json.dumps(data))
		self.assertEqual(response.status_code, 200)
		is_json,json_data = try_load_json(response.text)
		self.assertTrue(is_json,response.text)
		self.assertEqual(json_data['code'], 0, json_data['msg'])
		json_account = json_data['account']
		self.assertTrue(is_account(json_account),json_account)

		print(json_data)
		print("\n")
	
	'''
	{
	"code": 0,
	"msg": "OK",
	"hash": "CDE1EC247CAC41C321024DCEBF065662A46A49A119EF0641C0547111FBCB315D"
	}
	'''
	def test_send_block(self):
		data = {
			"action": "send_block",
			"from": Test_rpc.import_account,
			"to": Test_rpc.to_account,
			"amount": "1000000000000000000",
			"password": Test_rpc.import_password,
			"gas": "21000",
			"gas_price": "1000000000000",
			"data": ""
		}
		response = requests.post(url=URL, data=json.dumps(data))
		self.assertEqual(response.status_code, 200)
		is_json,json_data = try_load_json(response.text)
		self.assertTrue(is_json,response.text)
		json_data = json.loads(response.text)
		self.assertEqual(json_data['code'], 11, json_data['msg']) #code 11 means insufficient balance

	'''
	{
	"code": 0,
	"msg": "OK",
	"balances": {
		"mcp_33EuccjKjcZgwbHYp8eLhoFiaKGARVigZojeHzySD9fQ1ysd7u": "1000000000000000000", //1mcp
		"mcp_4m7NiSx2sBG4Hmdq1Yt6EGKqFQ3rmtBXCsmJZZp4E3pm84LkG9": "1000000000000000000"	 //1mcp
		}
	}
	'''
	def test_accounts_balances(self):
		data = {
			"action": "accounts_balances",
			"accounts": [
				Test_rpc.genesis_account,
				Test_rpc.import_account,
				Test_rpc.to_account
			]
		}
		response = requests.post(url=URL, data=json.dumps(data))
		self.assertEqual(response.status_code, 200)
		is_json,json_data = try_load_json(response.text)
		self.assertTrue(is_json,response.text)
		json_data = json.loads(response.text)
		self.assertEqual(json_data['code'], 0, json_data['msg'])
		json_balances = json_data['balances']
		self.assertTrue(len(json_balances)>0,json_balances)
		for i in json_balances:
			self.assertTrue(is_balance(i),json_balances)

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
			"account": "0x38369682E93F5C20A4DA3E374805C9584EA3E2C8"
		}
		response = requests.post(url=URL, data=json.dumps(data))
		self.assertEqual(response.status_code, 200)
		is_json,json_data = try_load_json(response.text)
		self.assertTrue(is_json,response.text)
		json_data = json.loads(response.text)
		self.assertEqual(json_data['code'], 0, json_data['msg'])
		json_balance = json_data['balance']
		self.assertTrue(is_balance(json_balance),json_balance)

		print(json_data)
		print("\n")
	
	'''
	{
    "code": 0,
    "msg": "OK",
    "output": "692A70D2E424A56D2C6C27AA97D1A86395877B3A2C6C27AA97D1A86395877B5C"
	}
	'''
	def test_call(self):
		data = {
			"action": "call",
			"from": Test_rpc.import_account,
			"to": Test_rpc.to_account
		}
		response = requests.post(url=URL, data=json.dumps(data))
		self.assertEqual(response.status_code, 200)
		is_json,json_data = try_load_json(response.text)
		self.assertTrue(is_json,response.text)
		json_data = json.loads(response.text)
		self.assertEqual(json_data['code'], 0, json_data['msg'])
		json_output = json_data['output']
		self.assertTrue(is_hex(json_output),json_output)
	
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
		response = requests.post(url=URL, data=json.dumps(data))
		self.assertEqual(response.status_code, 200)
		is_json,json_data = try_load_json(response.text)
		self.assertTrue(is_json,response.text)
		json_data = json.loads(response.text)
		self.assertEqual(json_data['code'], 0, json_data['msg'])
		json_account_code = json_data['account_code']
		self.assertTrue(is_hex(json_account_code),json_account_code)

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
		response = requests.post(url=URL, data=json.dumps(data))
		self.assertEqual(response.status_code, 200)
		is_json,json_data = try_load_json(response.text)
		self.assertTrue(is_json,response.text)
		json_data = json.loads(response.text)
		self.assertEqual(json_data['code'], 0, json_data['msg'])
	
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
	"json": "{\"account\":\"mcp_4GHUc91PYAddCiDHtwFnFDcsQrqdXW58Ps75rpHJsxAnJrQR1d\",\"kdf_salt\":\"37685A5B3413EC419CE4B5B79E0BB020\",\"iv\":\"F046EA90EA24A6CF0CB74BE8C560367B\",\"ciphertext\":\"4A2E6EE4CF04162D2A4DA6116C23CD94487837731055A1BC0FCBDA7E0D7C65A4\"}"
	}
	'''
	def test_account_export(self):
		data = {
			"action": "account_export",
			"account": Test_rpc.import_account
		}
		response = requests.post(url=URL, data=json.dumps(data))
		self.assertEqual(response.status_code, 200)
		is_json,json_data = try_load_json(response.text)
		self.assertTrue(is_json,response.text)
		json_data = json.loads(response.text)
		self.assertEqual(json_data['code'], 0, json_data['msg'])
		data_json = json_data['json']
		
		try:
			json_object = json.loads(data_json)
		except ValueError as e:
			self.assertTrue(False,e,data_json)
		
		json_account = json_object['account']
		self.assertTrue(is_account(json_account),json_account)
		
		json_kdf_salt = json_object['kdf_salt']
		self.assertTrue(is_hex(json_kdf_salt,32),json_kdf_salt)
		
		json_iv = json_object['iv']
		self.assertTrue(is_hex(json_iv,32),json_iv)
		
		json_ciphertext = json_object['ciphertext']
		self.assertTrue(is_hex(json_ciphertext,64),json_ciphertext)
	
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
		response = requests.post(url=URL, data=json.dumps(data))
		self.assertEqual(response.status_code, 200)
		is_json,json_data = try_load_json(response.text)
		self.assertTrue(is_json,response.text)
		json_data = json.loads(response.text)
		self.assertEqual(json_data['code'], 0, json_data['msg'])
		self.assertEqual(json_data['valid'], 1)
	
	'''
	{
	"code": 0,
	"msg": "OK"
	}  
	'''
	def test_account_password_change(self):
		new_password = "qwer1234"
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
			"limit": 100
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
	"gas": "21000"
	}
	'''
	def test_estimate_gas(self):
		data = {
			"action": "estimate_gas",
			"from": Test_rpc.import_account,
			"to": Test_rpc.to_account,
			"amount": "1000000000000000000",
			"gas": "21000",
			"gas_price": "1000000000000"
		}
		response = requests.post(url=URL, data=json.dumps(data))
		self.assertEqual(response.status_code, 200)
		is_json,json_data = try_load_json(response.text)
		self.assertTrue(is_json,response.text)
		json_data = json.loads(response.text)
		self.assertEqual(json_data['code'], 0, json_data['msg'])
		json_gas = json_data['gas']
		self.assertTrue(is_gas(json_gas),json_gas)

	def test_generate_offline_block(self):
		data = {
			"action": "generate_offline_block",
			"from": Test_rpc.genesis_account,
			"to": Test_rpc.import_account,
			"amount": "1000000000000000000",
			"gas": "21000",
			"gas_price": "1000000000",
			"data": ""
		}
		response = requests.post(url=URL, data=json.dumps(data))
		self.assertEqual(response.status_code, 200)
		is_json,json_data = try_load_json(response.text)
		self.assertTrue(is_json,response.text)
		json_data = json.loads(response.text)
		self.assertEqual(json_data['code'], 8, json_data['msg']) #code 8 means insufficient balance

	def test_send_offline_block(self):
		data = {
			"action": "send_offline_block",
			"previous": "0000000000000000000000000000000000000000000000000000000000000000",
			"from": Test_rpc.import_account,
			"to": Test_rpc.to_account,
			"amount": "1000000000000000000",
			"gas": "21000",
			"gas_price": "1000000000",
			"data": "",
			"signature": "71408627FF461C9DE076A38B71953A3045C95D1E1E841A2224E4AC3E503C0D0046FE8FEEB6E72B257B7743F53AFEC1CE80699D5E125C60794D6D09823C3B1E0C"
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
	"signature": "E09CDD795E6959C3B85FDCA0EA56BCFBBC7BE05A0D0AB6B1A0C6DD23FF0AA36F635C70CB731DAC07909A572132128120EBC12862D4BEC2FE70E9A6060F32CA0C"
	}
	'''
	def test_sign_msg(self):
		data = {
			"action": "sign_msg",
			"public_key": Test_rpc.import_public_key,
			"password": Test_rpc.import_password,
			"msg": "CB09A146D83668AE13E951032D2FD94F893C9A0CA0822ED40BBE11DC0F167D1B"
		}
		response = requests.post(url=URL, data=json.dumps(data))
		self.assertEqual(response.status_code, 200)
		is_json,json_data = try_load_json(response.text)
		self.assertTrue(is_json,response.text)
		json_data = json.loads(response.text)
		self.assertEqual(json_data['code'], 0, json_data['msg'])
		json_sign = json_data['signature']
		self.assertTrue(is_signature(json_sign),json_sign)

		print(json_data)
		print("\n")

	def test_block(self):
		data = {
			"action": "block",
			"hash": "412254AB895FD2E6ADE6F9076CA8297516F2845C989A13AC008CD5D70157AFFB"
		}
		response = requests.post(url=URL, data=json.dumps(data))
		self.assertEqual(response.status_code, 200)
		is_json,json_data = try_load_json(response.text)
		self.assertTrue(is_json,response.text)
		json_data = json.loads(response.text)
		self.assertEqual(json_data['code'], 0, json_data['msg'])
		
	def test_blocks(self):
		data = {
			"action": "blocks",
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

	'''
	{
	"code": 0,
    "msg": "OK",
    "hash": ""
	}
	'''
	def test_send_ccn(self):
		data = {
			"action": "send_block",
			"from": Test_rpc.genesis_account,
    		"to": Test_rpc.import_account,
			"amount": "1000000000000000000",
			"password": Test_rpc.import_password,
			"gas": "21000",
			"gas_price": "1000000000",
			"data": ""
		}
		response = requests.post(url=URL, data=json.dumps(data))
		self.assertEqual(response.status_code, 200)
		is_json,json_data = try_load_json(response.text)
		self.assertTrue(is_json,response.text)
		json_data = json.loads(response.text)
		self.assertEqual(json_data['code'], 0, json_data['msg'])
		
		print(json_data)
		print("\n")

if __name__ == "__main__":
	suite = unittest.TestSuite()
	# suite.addTest(Test_rpc("test_account_import"))
	# suite.addTest(Test_rpc("test_account_create"))
	# suite.addTest(Test_rpc("test_send_block"))
	suite.addTest(Test_rpc("test_accounts_balances"))
	# suite.addTest(Test_rpc("test_account_balance"))
	# suite.addTest(Test_rpc("test_call"))
	# suite.addTest(Test_rpc("test_account_code"))
	# suite.addTest(Test_rpc("test_account_lock"))
	# suite.addTest(Test_rpc("test_account_unlock"))
	# suite.addTest(Test_rpc("test_account_export"))
	# suite.addTest(Test_rpc("test_account_validate"))
	# suite.addTest(Test_rpc("test_account_password_change"))
	# suite.addTest(Test_rpc("test_account_list"))
	# suite.addTest(Test_rpc("test_account_block_list"))
	# suite.addTest(Test_rpc("test_estimate_gas"))
	suite.addTest(Test_rpc("test_generate_offline_block"))
	# suite.addTest(Test_rpc("test_send_offline_block"))
	# suite.addTest(Test_rpc("test_sign_msg"))
	# suite.addTest(Test_rpc("test_block"))
	# suite.addTest(Test_rpc("test_blocks"))
	# suite.addTest(Test_rpc("test_block_state"))
	# suite.addTest(Test_rpc("test_block_states"))
	# suite.addTest(Test_rpc("test_block_traces"))
	# suite.addTest(Test_rpc("test_stable_blocks"))
	# suite.addTest(Test_rpc("test_status"))
	# suite.addTest(Test_rpc("test_witness_list"))
	# suite.addTest(Test_rpc("test_version"))
	# suite.addTest(Test_rpc("test_account_remove"))
	# suite.addTest(Test_rpc("test_send_ccn"))

	result = unittest.TextTestRunner(verbosity=3).run(suite)
	if result.wasSuccessful():
		exit(0)
	else:
		exit(1)

