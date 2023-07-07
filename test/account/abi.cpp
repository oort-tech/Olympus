#include <mcp/common/numbers.hpp>
#include <mcp/wallet/wallet.hpp>
#include <mcp/wallet/key_manager.hpp>
#include <mcp/common/common.hpp>
#include <string>
#include <vector>
#include <mcp/common/assert.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/algorithm/string/join.hpp>
#include <mcp/core/contract.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>
#include <libdevcore/CommonJS.h>

void test_decode()
{
	///1 test return int,address,string,bool,bytes
	const std::string ABI1 = "[{\"inputs\":[{\"internalType\":\"int256\",\"name\":\"v1\",\"type\":\"int256\"},{\"internalType\":\"address\",\"name\":\"v2\",\"type\":\"address\"},{\"internalType\":\"string\",\"name\":\"v3\",\"type\":\"string\"},{\"internalType\":\"bool\",\"name\":\"v4\",\"type\":\"bool\"},{\"internalType\":\"bytes\",\"name\":\"v5\",\"type\":\"bytes\"}],\"name\":\"getreturns1\",\"outputs\":[{\"internalType\":\"int256\",\"name\":\"\",\"type\":\"int256\"},{\"internalType\":\"address\",\"name\":\"\",\"type\":\"address\"},{\"internalType\":\"string\",\"name\":\"\",\"type\":\"string\"},{\"internalType\":\"bool\",\"name\":\"\",\"type\":\"bool\"},{\"internalType\":\"bytes\",\"name\":\"\",\"type\":\"bytes\"}],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true}]";
	ABI abi1 = dev::JSON(ABI1);
	dev::bytes ret1 = dev::fromHex("0000000000000000000000000000000000000000000000000000000000000008000000000000000000000000d2b53c6dcf4eb754de108ec0420ee7ecfc1223b300000000000000000000000000000000000000000000000000000000000000a0000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000e00000000000000000000000000000000000000000000000000000000000000007616263646566670000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000030a0b0c0000000000000000000000000000000000000000000000000000000000");
	int v11; dev::h160 v12; std::string v13; bool v14; dev::bytes v15;
	abi1.Unpack("getreturns1", ret1, v11, v12, v13, v14, v15);
	//std::cout << "decode :" << v11 << " ," << v12 << " ," << v13 << " ," << v14 << " ," << v15 << std::endl;
	assert_x(v11==8 && toHex(v12)=="d2b53c6dcf4eb754de108ec0420ee7ecfc1223b3" && v13=="abcdefg" && v14==true && toHex(v15)=="0a0b0c");

	///2 test return int,address,string,bool,bytes array and slice
	///int, int array, int slice
	const std::string ABI2 = "[{\"inputs\":[{\"internalType\":\"int256\",\"name\":\"v1\",\"type\":\"int256\"},{\"internalType\":\"int256[2]\",\"name\":\"v2\",\"type\":\"int256[2]\"},{\"internalType\":\"int256[]\",\"name\":\"v3\",\"type\":\"int256[]\"}],\"name\":\"getreturns21\",\"outputs\":[{\"internalType\":\"int256\",\"name\":\"\",\"type\":\"int256\"},{\"internalType\":\"int256[2]\",\"name\":\"\",\"type\":\"int256[2]\"},{\"internalType\":\"int256[]\",\"name\":\"\",\"type\":\"int256[]\"}],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"internalType\":\"address\",\"name\":\"v1\",\"type\":\"address\"},{\"internalType\":\"address[2]\",\"name\":\"v2\",\"type\":\"address[2]\"},{\"internalType\":\"address[]\",\"name\":\"v3\",\"type\":\"address[]\"}],\"name\":\"getreturns22\",\"outputs\":[{\"internalType\":\"address\",\"name\":\"\",\"type\":\"address\"},{\"internalType\":\"address[2]\",\"name\":\"\",\"type\":\"address[2]\"},{\"internalType\":\"address[]\",\"name\":\"\",\"type\":\"address[]\"}],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"internalType\":\"string\",\"name\":\"v1\",\"type\":\"string\"},{\"internalType\":\"string[2]\",\"name\":\"v2\",\"type\":\"string[2]\"},{\"internalType\":\"string[]\",\"name\":\"v3\",\"type\":\"string[]\"}],\"name\":\"getreturns23\",\"outputs\":[{\"internalType\":\"string\",\"name\":\"\",\"type\":\"string\"},{\"internalType\":\"string[2]\",\"name\":\"\",\"type\":\"string[2]\"},{\"internalType\":\"string[]\",\"name\":\"\",\"type\":\"string[]\"}],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"internalType\":\"bool\",\"name\":\"v1\",\"type\":\"bool\"},{\"internalType\":\"bool[2]\",\"name\":\"v2\",\"type\":\"bool[2]\"},{\"internalType\":\"bool[]\",\"name\":\"v3\",\"type\":\"bool[]\"}],\"name\":\"getreturns24\",\"outputs\":[{\"internalType\":\"bool\",\"name\":\"\",\"type\":\"bool\"},{\"internalType\":\"bool[2]\",\"name\":\"\",\"type\":\"bool[2]\"},{\"internalType\":\"bool[]\",\"name\":\"\",\"type\":\"bool[]\"}],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"internalType\":\"bytes\",\"name\":\"v1\",\"type\":\"bytes\"},{\"internalType\":\"bytes[2]\",\"name\":\"v2\",\"type\":\"bytes[2]\"},{\"internalType\":\"bytes[]\",\"name\":\"v3\",\"type\":\"bytes[]\"}],\"name\":\"getreturns25\",\"outputs\":[{\"internalType\":\"bytes\",\"name\":\"\",\"type\":\"bytes\"},{\"internalType\":\"bytes[2]\",\"name\":\"\",\"type\":\"bytes[2]\"},{\"internalType\":\"bytes[]\",\"name\":\"\",\"type\":\"bytes[]\"}],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"internalType\":\"bytes3\",\"name\":\"v1\",\"type\":\"bytes3\"},{\"internalType\":\"bytes3[2]\",\"name\":\"v2\",\"type\":\"bytes3[2]\"},{\"internalType\":\"bytes3[]\",\"name\":\"v3\",\"type\":\"bytes3[]\"}],\"name\":\"getreturns26\",\"outputs\":[{\"internalType\":\"bytes3\",\"name\":\"\",\"type\":\"bytes3\"},{\"internalType\":\"bytes3[2]\",\"name\":\"\",\"type\":\"bytes3[2]\"},{\"internalType\":\"bytes3[]\",\"name\":\"\",\"type\":\"bytes3[]\"}],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"internalType\":\"int256[]\",\"name\":\"v1\",\"type\":\"int256[]\"},{\"internalType\":\"address[]\",\"name\":\"v2\",\"type\":\"address[]\"},{\"internalType\":\"string[]\",\"name\":\"v3\",\"type\":\"string[]\"},{\"internalType\":\"bool[]\",\"name\":\"v4\",\"type\":\"bool[]\"},{\"internalType\":\"bytes[]\",\"name\":\"v5\",\"type\":\"bytes[]\"}],\"name\":\"getreturns27\",\"outputs\":[{\"internalType\":\"int256[]\",\"name\":\"\",\"type\":\"int256[]\"},{\"internalType\":\"address[]\",\"name\":\"\",\"type\":\"address[]\"},{\"internalType\":\"string[]\",\"name\":\"\",\"type\":\"string[]\"},{\"internalType\":\"bool[]\",\"name\":\"\",\"type\":\"bool[]\"},{\"internalType\":\"bytes[]\",\"name\":\"\",\"type\":\"bytes[]\"}],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true}]";
	ABI abi2 = dev::JSON(ABI2);
	dev::bytes ret21 = dev::fromHex("000000000000000000000000000000000000000000000000000000000000000b000000000000000000000000000000000000000000000000000000000000000c000000000000000000000000000000000000000000000000000000000000000d00000000000000000000000000000000000000000000000000000000000000800000000000000000000000000000000000000000000000000000000000000003000000000000000000000000000000000000000000000000000000000000000e000000000000000000000000000000000000000000000000000000000000000f0000000000000000000000000000000000000000000000000000000000000010");
	int i21;///11
	std::vector<int> arrint21; ///12,13
	std::vector<int> arrint22; ///14,15,16
	abi2.Unpack("getreturns21",ret21, i21, arrint21, arrint22);
	//std::cout << "decode :" << i21 << std::endl;
	//for (auto v : arrint21)
	//	std::cout << "decode :" << v << std::endl;
	//for (auto v : arrint22)
	//	std::cout << "decode :" << v << std::endl;
	assert_x(i21 == 11 && 
		arrint21.size() == 2 && arrint21[0] == 12 && arrint21[1] == 13 &&
		arrint22.size() == 3 && arrint22[0] == 14 && arrint22[1] == 15 && arrint22[2] == 16);

	/// address, address array, address slice
	dev::bytes ret22 = dev::fromHex("000000000000000000000000d2b53c6dcf4eb754de108ec0420ee7ecfc1223b3000000000000000000000000329e6b5b8e59fc73d892958b2ff6a89474e3d067000000000000000000000000827cce78dc6ec7051f2d7bb9e7adaefba7ca32480000000000000000000000000000000000000000000000000000000000000080000000000000000000000000000000000000000000000000000000000000000300000000000000000000000049a1b41e8ccb704f5c069ef89b08cd33f764e9b3000000000000000000000000f0821dc4ba9419b865aa412170377ca3b44cdb58000000000000000000000000dab8a5fb82eb24ad321751bb2dd8e4cc9a4e45e5");
	dev::h160 address21;
	std::vector<dev::h160> arraddress22;
	std::vector<dev::h160> arraddress23;
	abi2.Unpack("getreturns22", ret22, address21, arraddress22, arraddress23);
	//std::cout << "decode :" << address21.hex() << std::endl;
	//for (auto v : arraddress22)
	//	std::cout << "decode :" << v.hex() << std::endl;
	//for (auto v : arraddress23)
	//	std::cout << "decode :" << v.hex() << std::endl;
	assert_x(address21.hex() == "d2b53c6dcf4eb754de108ec0420ee7ecfc1223b3" &&
		arraddress22.size() == 2 && arraddress22[0].hex() == "329e6b5b8e59fc73d892958b2ff6a89474e3d067" && 
		arraddress22[1].hex() == "827cce78dc6ec7051f2d7bb9e7adaefba7ca3248" &&
		arraddress23.size() == 3 && arraddress23[0].hex() == "49a1b41e8ccb704f5c069ef89b08cd33f764e9b3" && 
		arraddress23[1].hex() == "f0821dc4ba9419b865aa412170377ca3b44cdb58" &&
		arraddress23[2].hex() == "dab8a5fb82eb24ad321751bb2dd8e4cc9a4e45e5"
	);
	
	/// string, string array, string slice
	dev::bytes ret23 = dev::fromHex("000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000e000000000000000000000000000000000000000000000000000000000000001e000000000000000000000000000000000000000000000000000000000000000516162636465666768696a6b6c6d6e6f707172737475767778797a6162636465666768696a6b6c6d6e6f707172737475767778797a6162636465666768696a6b6c6d6e6f707172737475767778797a313131000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000c000000000000000000000000000000000000000000000000000000000000000516162636465666768696a6b6c6d6e6f707172737475767778797a6162636465666768696a6b6c6d6e6f707172737475767778797a6162636465666768696a6b6c6d6e6f707172737475767778797a323232000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000561626364650000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000e0000000000000000000000000000000000000000000000000000000000000012000000000000000000000000000000000000000000000000000000000000000516162636465666768696a6b6c6d6e6f707172737475767778797a6162636465666768696a6b6c6d6e6f707172737475767778797a6162636465666768696a6b6c6d6e6f707172737475767778797a3333330000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003616263000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000056465666768000000000000000000000000000000000000000000000000000000");
	std::string string21;
	std::vector<std::string> arrstring22;
	std::vector<std::string> arrstring23;
	abi2.Unpack("getreturns23", ret23, string21, arrstring22, arrstring23);
	//std::cout << "decode :" << string21 << std::endl;
	//for (auto v : arrstring22)
	//	std::cout << "decode :" << v << std::endl;
	//for (auto v : arrstring23)
	//	std::cout << "decode :" << v << std::endl;
	assert_x(string21 == "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz111" &&
		arrstring22.size() == 2 && arrstring22[0] == "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz222" &&
		arrstring22[1] == "abcde" &&
		arrstring23.size() == 3 && arrstring23[0] == "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz333" &&
		arrstring23[1] == "abc" &&
		arrstring23[2] == "defgh"
	);

	/// bool, bool array, bool slice
	dev::bytes ret24 = dev::fromHex("00000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000800000000000000000000000000000000000000000000000000000000000000003000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001");
	bool bool21;
	std::vector<bool> arrbool22;
	std::vector<bool> arrbool23;
	abi2.Unpack("getreturns24", ret24, bool21, arrbool22, arrbool23);
	//std::cout << "decode :" << bool21 << std::endl;
	//for (auto v : arrbool22)
	//	std::cout << "decode :" << v << std::endl;
	//for (auto v : arrbool23)
	//	std::cout << "decode :" << v << std::endl;
	assert_x(bool21 &&
		arrbool22.size() == 2 && arrbool22[0] && !arrbool22[1] &&
		arrbool23.size() == 3 && arrbool23[0] && !arrbool23[1] && arrbool23[2]
	);

	/// bytes, bytes array, bytes slice
	dev::bytes ret25 = dev::fromHex("000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000c000000000000000000000000000000000000000000000000000000000000001a000000000000000000000000000000000000000000000000000000000000000240102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c01010100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000a000000000000000000000000000000000000000000000000000000000000000240102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c0202020000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000045c020202000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000c0000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000210102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000045c0303030000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000065d5e5f0303030000000000000000000000000000000000000000000000000000");
	dev::bytes bytes21;
	std::vector<dev::bytes> arrbytes22;
	std::vector<dev::bytes> arrbytes23;
	abi2.Unpack("getreturns25", ret25, bytes21, arrbytes22, arrbytes23);
	//std::cout << "decode:" << toHex(bytes21) << std::endl;
	//for (auto v : arrbytes22)
	//	std::cout << "decode:" << toHex(v) << std::endl;
	//for (auto v : arrbytes23)
	//	std::cout << "decode:" << toHex(v) << std::endl;
	assert_x(toHex(bytes21) == "0102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c010101" &&
		arrbytes22.size() == 2 && toHex(arrbytes22[0]) == "0102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c020202" &&
		toHex(arrbytes22[1]) == "5c020202" &&
		arrbytes23.size() == 3 && toHex(arrbytes23[0]) == "0102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c" &&
		toHex(arrbytes23[1]) == "5c030303" &&
		toHex(arrbytes23[2]) == "5d5e5f030303"
	);

	/// bytes3, bytes3 array, bytes3 slice
	dev::bytes ret26 = dev::fromHex("01010100000000000000000000000000000000000000000000000000000000000202020000000000000000000000000000000000000000000000000000000000040506000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000800000000000000000000000000000000000000000000000000000000000000003030303000000000000000000000000000000000000000000000000000000000024252600000000000000000000000000000000000000000000000000000000000a0b0c0000000000000000000000000000000000000000000000000000000000");
	dev::bytes bytes321;
	std::vector<dev::bytes> arrbytes322;
	std::vector<dev::bytes> arrbytes323;
	abi2.Unpack("getreturns26", ret26, bytes321, arrbytes322, arrbytes323);
	//std::cout << "decode:" << toHex(bytes321) << std::endl;
	//for (auto v : arrbytes322)
	//	std::cout << "decode:" << toHex(v) << std::endl;
	//for (auto v : arrbytes323)
	//	std::cout << "decode:" << toHex(v) << std::endl;
	assert_x(toHex(bytes321) == "010101" &&
		arrbytes322.size() == 2 && toHex(arrbytes322[0]) == "020202" &&
		toHex(arrbytes322[1]) == "040506" &&
		arrbytes323.size() == 3 && toHex(arrbytes323[0]) == "030303" &&
		toHex(arrbytes323[1]) == "242526" &&
		toHex(arrbytes323[2]) == "0a0b0c"
	);

	/// all slice
	dev::bytes ret27 = dev::fromHex("00000000000000000000000000000000000000000000000000000000000000a0000000000000000000000000000000000000000000000000000000000000012000000000000000000000000000000000000000000000000000000000000001a0000000000000000000000000000000000000000000000000000000000000032000000000000000000000000000000000000000000000000000000000000003a00000000000000000000000000000000000000000000000000000000000000003000000000000000000000000000000000000000000000000000000000000000e000000000000000000000000000000000000000000000000000000000000000f0000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000300000000000000000000000049a1b41e8ccb704f5c069ef89b08cd33f764e9b3000000000000000000000000f0821dc4ba9419b865aa412170377ca3b44cdb58000000000000000000000000dab8a5fb82eb24ad321751bb2dd8e4cc9a4e45e50000000000000000000000000000000000000000000000000000000000000003000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000e0000000000000000000000000000000000000000000000000000000000000012000000000000000000000000000000000000000000000000000000000000000516162636465666768696a6b6c6d6e6f707172737475767778797a6162636465666768696a6b6c6d6e6f707172737475767778797a6162636465666768696a6b6c6d6e6f707172737475767778797a333333000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000361626300000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000005646566676800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000030000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000003000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000c0000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000210102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000045c0303030000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000065d5e5f0303030000000000000000000000000000000000000000000000000000");
	std::vector<int> arrint271;
	std::vector<dev::h160> arraddress272;
	std::vector<std::string> arrstring273;
	std::vector<bool> arrboo274;
	std::vector<dev::bytes> arrbytes275;
	abi2.Unpack("getreturns27", ret27, arrint271, arraddress272, arrstring273, arrboo274, arrbytes275);
	//for (auto v : arrint271)
	//	std::cout << "decode :" << v << std::endl;
	//for (auto v : arraddress272)
	//	std::cout << "decode :" << v.hex() << std::endl;
	//for (auto v : arrstring273)
	//	std::cout << "decode :" << v << std::endl;
	//for (auto v : arrboo274)
	//	std::cout << "decode :" << v << std::endl;
	//for (auto v : arrbytes275)
	//	std::cout << "decode:" << toHex(v) << std::endl;
	assert_x(arrint271.size() == 3 && arrint271[0] == 14 && arrint271[1] == 15 && arrint271[2] == 16 &&
		arraddress272.size() == 3 && arraddress272[0].hex() == "49a1b41e8ccb704f5c069ef89b08cd33f764e9b3" &&
			arraddress272[1].hex() == "f0821dc4ba9419b865aa412170377ca3b44cdb58" &&
			arraddress272[2].hex() == "dab8a5fb82eb24ad321751bb2dd8e4cc9a4e45e5" &&
		arrstring273.size() == 3 && arrstring273[0] == "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz333" &&
			arrstring273[1] == "abc" &&
			arrstring273[2] == "defgh" &&
		arrboo274.size() == 3 && arrboo274[0] && !arrboo274[1] && arrboo274[2] &&
		arrbytes275.size() == 3 && toHex(arrbytes275[0]) == "0102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c" &&
			toHex(arrbytes275[1]) == "5c030303" &&
			toHex(arrbytes275[2]) == "5d5e5f030303"
	);

	///3 test tuple
	//const std::string ABI3 = "[{\"inputs\":[{\"internalType\":\"int256\",\"name\":\"v1\",\"type\":\"int256\"},{\"internalType\":\"address\",\"name\":\"v2\",\"type\":\"address\"},{\"internalType\":\"string\",\"name\":\"v3\",\"type\":\"string\"},{\"internalType\":\"bool\",\"name\":\"v4\",\"type\":\"bool\"},{\"internalType\":\"bytes\",\"name\":\"v5\",\"type\":\"bytes\"}],\"name\":\"getreturns1\",\"outputs\":[{\"internalType\":\"int256\",\"name\":\"\",\"type\":\"int256\"},{\"internalType\":\"address\",\"name\":\"\",\"type\":\"address\"},{\"internalType\":\"string\",\"name\":\"\",\"type\":\"string\"},{\"internalType\":\"bool\",\"name\":\"\",\"type\":\"bool\"},{\"internalType\":\"bytes\",\"name\":\"\",\"type\":\"bytes\"}],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"components\":[{\"internalType\":\"int256\",\"name\":\"intstores\",\"type\":\"int256\"},{\"internalType\":\"address\",\"name\":\"addressstores\",\"type\":\"address\"},{\"internalType\":\"string\",\"name\":\"stringstores\",\"type\":\"string\"},{\"internalType\":\"bool\",\"name\":\"boolstores\",\"type\":\"bool\"},{\"internalType\":\"bytes\",\"name\":\"bytesstores\",\"type\":\"bytes\"}],\"internalType\":\"struct Store.st4\",\"name\":\"v1\",\"type\":\"tuple\"}],\"name\":\"getreturnstuple\",\"outputs\":[{\"components\":[{\"internalType\":\"int256\",\"name\":\"intstores\",\"type\":\"int256\"},{\"internalType\":\"address\",\"name\":\"addressstores\",\"type\":\"address\"},{\"internalType\":\"string\",\"name\":\"stringstores\",\"type\":\"string\"},{\"internalType\":\"bool\",\"name\":\"boolstores\",\"type\":\"bool\"},{\"internalType\":\"bytes\",\"name\":\"bytesstores\",\"type\":\"bytes\"}],\"internalType\":\"struct Store.st4\",\"name\":\"\",\"type\":\"tuple\"}],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"components\":[{\"internalType\":\"int256[2]\",\"name\":\"intstores\",\"type\":\"int256[2]\"},{\"internalType\":\"address[2]\",\"name\":\"addressstores\",\"type\":\"address[2]\"},{\"internalType\":\"string[2]\",\"name\":\"stringstores\",\"type\":\"string[2]\"},{\"internalType\":\"bool[2]\",\"name\":\"boolstores\",\"type\":\"bool[2]\"},{\"internalType\":\"bytes[2]\",\"name\":\"bytesstores\",\"type\":\"bytes[2]\"},{\"internalType\":\"bytes3[2]\",\"name\":\"bytes3stores\",\"type\":\"bytes3[2]\"}],\"internalType\":\"struct Store.st5\",\"name\":\"v1\",\"type\":\"tuple\"}],\"name\":\"getreturnstuple2\",\"outputs\":[{\"components\":[{\"internalType\":\"int256[2]\",\"name\":\"intstores\",\"type\":\"int256[2]\"},{\"internalType\":\"address[2]\",\"name\":\"addressstores\",\"type\":\"address[2]\"},{\"internalType\":\"string[2]\",\"name\":\"stringstores\",\"type\":\"string[2]\"},{\"internalType\":\"bool[2]\",\"name\":\"boolstores\",\"type\":\"bool[2]\"},{\"internalType\":\"bytes[2]\",\"name\":\"bytesstores\",\"type\":\"bytes[2]\"},{\"internalType\":\"bytes3[2]\",\"name\":\"bytes3stores\",\"type\":\"bytes3[2]\"}],\"internalType\":\"struct Store.st5\",\"name\":\"\",\"type\":\"tuple\"}],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true}]";
	const std::string ABI3 = "[{\"inputs\":[{\"internalType\":\"int256\",\"name\":\"v1\",\"type\":\"int256\"},{\"internalType\":\"address\",\"name\":\"v2\",\"type\":\"address\"},{\"internalType\":\"string\",\"name\":\"v3\",\"type\":\"string\"},{\"internalType\":\"bool\",\"name\":\"v4\",\"type\":\"bool\"},{\"internalType\":\"bytes\",\"name\":\"v5\",\"type\":\"bytes\"}],\"name\":\"getreturns1\",\"outputs\":[{\"internalType\":\"int256\",\"name\":\"\",\"type\":\"int256\"},{\"internalType\":\"address\",\"name\":\"\",\"type\":\"address\"},{\"internalType\":\"string\",\"name\":\"\",\"type\":\"string\"},{\"internalType\":\"bool\",\"name\":\"\",\"type\":\"bool\"},{\"internalType\":\"bytes\",\"name\":\"\",\"type\":\"bytes\"}],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"components\":[{\"internalType\":\"int256\",\"name\":\"intstores\",\"type\":\"int256\"},{\"internalType\":\"address\",\"name\":\"addressstores\",\"type\":\"address\"},{\"internalType\":\"string\",\"name\":\"stringstores\",\"type\":\"string\"},{\"internalType\":\"bool\",\"name\":\"boolstores\",\"type\":\"bool\"},{\"internalType\":\"bytes\",\"name\":\"bytesstores\",\"type\":\"bytes\"}],\"internalType\":\"struct Store.st4\",\"name\":\"v1\",\"type\":\"tuple\"}],\"name\":\"getreturnstuple\",\"outputs\":[{\"components\":[{\"internalType\":\"int256\",\"name\":\"intstores\",\"type\":\"int256\"},{\"internalType\":\"address\",\"name\":\"addressstores\",\"type\":\"address\"},{\"internalType\":\"string\",\"name\":\"stringstores\",\"type\":\"string\"},{\"internalType\":\"bool\",\"name\":\"boolstores\",\"type\":\"bool\"},{\"internalType\":\"bytes\",\"name\":\"bytesstores\",\"type\":\"bytes\"}],\"internalType\":\"struct Store.st4\",\"name\":\"\",\"type\":\"tuple\"}],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"components\":[{\"internalType\":\"int256[2]\",\"name\":\"intstores\",\"type\":\"int256[2]\"},{\"internalType\":\"address[2]\",\"name\":\"addressstores\",\"type\":\"address[2]\"},{\"internalType\":\"string[2]\",\"name\":\"stringstores\",\"type\":\"string[2]\"},{\"internalType\":\"bool[2]\",\"name\":\"boolstores\",\"type\":\"bool[2]\"},{\"internalType\":\"bytes[2]\",\"name\":\"bytesstores\",\"type\":\"bytes[2]\"},{\"internalType\":\"bytes3[2]\",\"name\":\"bytes3stores\",\"type\":\"bytes3[2]\"}],\"internalType\":\"struct Store.st5\",\"name\":\"v1\",\"type\":\"tuple\"}],\"name\":\"getreturnstuple2\",\"outputs\":[{\"components\":[{\"internalType\":\"int256[2]\",\"name\":\"intstores\",\"type\":\"int256[2]\"},{\"internalType\":\"address[2]\",\"name\":\"addressstores\",\"type\":\"address[2]\"},{\"internalType\":\"string[2]\",\"name\":\"stringstores\",\"type\":\"string[2]\"},{\"internalType\":\"bool[2]\",\"name\":\"boolstores\",\"type\":\"bool[2]\"},{\"internalType\":\"bytes[2]\",\"name\":\"bytesstores\",\"type\":\"bytes[2]\"},{\"internalType\":\"bytes3[2]\",\"name\":\"bytes3stores\",\"type\":\"bytes3[2]\"}],\"internalType\":\"struct Store.st5\",\"name\":\"\",\"type\":\"tuple\"}],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"components\":[{\"internalType\":\"int256[]\",\"name\":\"intstores\",\"type\":\"int256[]\"},{\"internalType\":\"address[]\",\"name\":\"addressstores\",\"type\":\"address[]\"},{\"internalType\":\"string[]\",\"name\":\"stringstores\",\"type\":\"string[]\"},{\"internalType\":\"bool[]\",\"name\":\"boolstores\",\"type\":\"bool[]\"},{\"internalType\":\"bytes[]\",\"name\":\"bytesstores\",\"type\":\"bytes[]\"},{\"internalType\":\"bytes3[]\",\"name\":\"bytes3stores\",\"type\":\"bytes3[]\"}],\"internalType\":\"struct Store.st6\",\"name\":\"v1\",\"type\":\"tuple\"}],\"name\":\"getreturnstuple3\",\"outputs\":[{\"components\":[{\"internalType\":\"int256[]\",\"name\":\"intstores\",\"type\":\"int256[]\"},{\"internalType\":\"address[]\",\"name\":\"addressstores\",\"type\":\"address[]\"},{\"internalType\":\"string[]\",\"name\":\"stringstores\",\"type\":\"string[]\"},{\"internalType\":\"bool[]\",\"name\":\"boolstores\",\"type\":\"bool[]\"},{\"internalType\":\"bytes[]\",\"name\":\"bytesstores\",\"type\":\"bytes[]\"},{\"internalType\":\"bytes3[]\",\"name\":\"bytes3stores\",\"type\":\"bytes3[]\"}],\"internalType\":\"struct Store.st6\",\"name\":\"\",\"type\":\"tuple\"}],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true}]";
	ABI abi3 = dev::JSON(ABI3);
	dev::bytes ret31 = dev::fromHex("00000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000000000000000000000064000000000000000000000000329e6b5b8e59fc73d892958b2ff6a89474e3d06700000000000000000000000000000000000000000000000000000000000000a0000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000e00000000000000000000000000000000000000000000000000000000000000007616263646566670000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000210102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c00000000000000000000000000000000000000000000000000000000000000");
	struct tuple3
	{
		int int1;
		dev::h160 address1;
		std::string string1;
		bool bool1;
		dev::bytes bytes1;
	};
	tuple3 t3;
	auto p = boost::make_tuple(boost::ref(t3.int1), boost::ref(t3.address1), boost::ref(t3.string1), boost::ref(t3.bool1), boost::ref(t3.bytes1));
	abi3.Unpack("getreturnstuple", ret31, p);
	//std::cout << "decode :" << t3.int1 << std::endl;
	//std::cout << "decode :" << t3.address1.hex() << std::endl;
	//std::cout << "decode :" << t3.string1 << std::endl;
	//std::cout << "decode :" << t3.bool1 << std::endl;
	//std::cout << "decode :" << toHex(t3.bytes1) << std::endl;
	assert_x(
		t3.int1 == 100 &&
		t3.address1.hex() == "329e6b5b8e59fc73d892958b2ff6a89474e3d067" &&
		t3.string1 == "abcdefg" &&
		t3.bool1 &&
		toHex(t3.bytes1) == "0102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c"
	);

	///st using array test tuple
	dev::bytes ret32 = dev::fromHex("000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000002000000000000000000000000329e6b5b8e59fc73d892958b2ff6a89474e3d067000000000000000000000000827cce78dc6ec7051f2d7bb9e7adaefba7ca3248000000000000000000000000000000000000000000000000000000000000014000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000024021222300000000000000000000000000000000000000000000000000000000002425260000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000c0000000000000000000000000000000000000000000000000000000000000004e6162636465666768696a6b6c6d6e6f707172737475767778797a6162636465666768696a6b6c6d6e6f707172737475767778797a6162636465666768696a6b6c6d6e6f707172737475767778797a00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000036161610000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000a000000000000000000000000000000000000000000000000000000000000000210102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000036a6b6c0000000000000000000000000000000000000000000000000000000000");
	struct tuple32
	{
		std::vector<int> int1;
		std::vector<dev::h160> address1;
		std::vector<std::string> string1;
		std::vector<bool> bool1;
		std::vector<dev::bytes> bytes1;
		std::vector<dev::bytes> bytes2;
	};
	tuple32 t32;
	auto p32 = boost::make_tuple(boost::ref(t32.int1), boost::ref(t32.address1), boost::ref(t32.string1), boost::ref(t32.bool1), boost::ref(t32.bytes1), boost::ref(t32.bytes2));
	abi3.Unpack("getreturnstuple2", ret32, p32);
	//for (auto v : t32.int1)
	//	std::cout << "decode :" << v << std::endl;
	//for (auto v : t32.address1)
	//	std::cout << "decode :" << v.hex() << std::endl;
	//for (auto v : t32.string1)
	//	std::cout << "decode :" << v << std::endl;
	//for (auto v : t32.bool1)
	//	std::cout << "decode :" << v << std::endl;
	//for (auto v : t32.bytes1)
	//	std::cout << "decode :" << toHex(v) << std::endl;
	//for (auto v : t32.bytes2)
	//	std::cout << "decode :" << toHex(v) << std::endl;
	assert_x(t32.int1.size() == 2 && t32.address1.size() == 2 && t32.string1.size() == 2 && t32.bool1.size() == 2 && t32.bytes1.size() == 2 && t32.bytes2.size() == 2 &&
		t32.int1[0] == 1 && t32.int1[1] == 2 &&
		t32.address1[0].hex() == "329e6b5b8e59fc73d892958b2ff6a89474e3d067" && t32.address1[1].hex() == "827cce78dc6ec7051f2d7bb9e7adaefba7ca3248" &&
		t32.string1[0] == "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz" &&t32.string1[1] == "aaa" &&
		t32.bool1[0] && !t32.bool1[1] &&
		toHex(t32.bytes1[0]) == "0102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c" && toHex(t32.bytes1[1]) == "6a6b6c" &&
		toHex(t32.bytes2[0]) == "212223" && toHex(t32.bytes2[1]) == "242526"
	);

	///st using slice test tuple
	dev::bytes ret33 = dev::fromHex("000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000000c00000000000000000000000000000000000000000000000000000000000000120000000000000000000000000000000000000000000000000000000000000018000000000000000000000000000000000000000000000000000000000000002a0000000000000000000000000000000000000000000000000000000000000030000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000000000000000002000000000000000000000000329e6b5b8e59fc73d892958b2ff6a89474e3d067000000000000000000000000827cce78dc6ec7051f2d7bb9e7adaefba7ca32480000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000c0000000000000000000000000000000000000000000000000000000000000004e6162636465666768696a6b6c6d6e6f707172737475767778797a6162636465666768696a6b6c6d6e6f707172737475767778797a6162636465666768696a6b6c6d6e6f707172737475767778797a000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000361616100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000a000000000000000000000000000000000000000000000000000000000000000210102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000036a6b6c0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000221222300000000000000000000000000000000000000000000000000000000002425260000000000000000000000000000000000000000000000000000000000");
	tuple32 t33;
	auto p33 = boost::make_tuple(boost::ref(t33.int1), boost::ref(t33.address1), boost::ref(t33.string1), boost::ref(t33.bool1), boost::ref(t33.bytes1), boost::ref(t33.bytes2));
	abi3.Unpack("getreturnstuple2", ret32, p33);
	//for (auto v : t33.int1)
	//	std::cout << "decode :" << v << std::endl;
	//for (auto v : t33.address1)
	//	std::cout << "decode :" << v.hex() << std::endl;
	//for (auto v : t33.string1)
	//	std::cout << "decode :" << v << std::endl;
	//for (auto v : t33.bool1)
	//	std::cout << "decode :" << v << std::endl;
	//for (auto v : t33.bytes1)
	//	std::cout << "decode :" << toHex(v) << std::endl;
	//for (auto v : t33.bytes2)
	//	std::cout << "decode :" << toHex(v) << std::endl;
	assert_x(t33.int1.size() == 2 && t33.address1.size() == 2 && t33.string1.size() == 2 && t33.bool1.size() == 2 && t33.bytes1.size() == 2 && t33.bytes2.size() == 2 &&
		t33.int1[0] == 1 && t33.int1[1] == 2 &&
		t33.address1[0].hex() == "329e6b5b8e59fc73d892958b2ff6a89474e3d067" && t33.address1[1].hex() == "827cce78dc6ec7051f2d7bb9e7adaefba7ca3248" &&
		t33.string1[0] == "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz" &&t33.string1[1] == "aaa" &&
		t33.bool1[0] && !t33.bool1[1] &&
		toHex(t33.bytes1[0]) == "0102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c" && toHex(t33.bytes1[1]) == "6a6b6c" &&
		toHex(t33.bytes2[0]) == "212223" && toHex(t33.bytes2[1]) == "242526"
	);
}

void test_abi()
{
	/// 1 test void
	const std::string ABI0 = "[{\"inputs\":[],\"name\":\"intstores\",\"outputs\":[{\"internalType\":\"int256\",\"name\":\"\",\"type\":\"int256\"}],\"stateMutability\":\"view\",\"type\":\"function\"},{\"inputs\":[{\"internalType\":\"int256\",\"name\":\"value\",\"type\":\"int256\"}],\"name\":\"Set\",\"outputs\":[],\"stateMutability\":\"nonpayable\",\"type\":\"function\"},{\"inputs\":[],\"name\":\"Get\",\"outputs\":[{\"internalType\":\"int256\",\"name\":\"\",\"type\":\"int256\"}],\"stateMutability\":\"view\",\"type\":\"function\"}]";
	ABI abi0 = dev::JSON(ABI0);
	//std::cout << toHex(abi0.Pack("Get")) << std::endl;
	assert_x(toHex(abi0.Pack("Get")) == "b1976a02");

	///1 test int
	const std::string ABI1 = "[{\"inputs\":[{\"internalType\":\"int256\",\"name\":\"\",\"type\":\"int256\"}],\"name\":\"intstores\",\"outputs\":[{\"internalType\":\"int256\",\"name\":\"\",\"type\":\"int256\"}],\"stateMutability\":\"view\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"internalType\":\"int256\",\"name\":\"value\",\"type\":\"int256\"}],\"name\":\"setint\",\"outputs\":[],\"stateMutability\":\"nonpayable\",\"type\":\"function\"},{\"inputs\":[{\"internalType\":\"int256\",\"name\":\"value\",\"type\":\"int256\"}],\"name\":\"getint\",\"outputs\":[{\"internalType\":\"int256\",\"name\":\"\",\"type\":\"int256\"}],\"stateMutability\":\"view\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"internalType\":\"uint256\",\"name\":\"value\",\"type\":\"uint256\"}],\"name\":\"getuint\",\"outputs\":[{\"internalType\":\"uint256\",\"name\":\"\",\"type\":\"uint256\"}],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"internalType\":\"int8\",\"name\":\"value\",\"type\":\"int8\"}],\"name\":\"getint8\",\"outputs\":[{\"internalType\":\"int8\",\"name\":\"\",\"type\":\"int8\"}],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"internalType\":\"int40\",\"name\":\"value\",\"type\":\"int40\"}],\"name\":\"getint40\",\"outputs\":[{\"internalType\":\"int40\",\"name\":\"\",\"type\":\"int40\"}],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"internalType\":\"int64\",\"name\":\"value\",\"type\":\"int64\"}],\"name\":\"getint64\",\"outputs\":[{\"internalType\":\"int64\",\"name\":\"\",\"type\":\"int64\"}],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"internalType\":\"int128\",\"name\":\"value\",\"type\":\"int128\"}],\"name\":\"getint128\",\"outputs\":[{\"internalType\":\"int128\",\"name\":\"\",\"type\":\"int128\"}],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true}]";
	ABI abi1 = dev::JSON(ABI1);
	assert_x(toHex(abi1.Pack("getint", 1)) == "c340e1bd0000000000000000000000000000000000000000000000000000000000000001");
	assert_x(toHex(abi1.Pack("setint", 1)) == "d24ce25a0000000000000000000000000000000000000000000000000000000000000001");
	assert_x(toHex(abi1.Pack("setint", 10)) == "d24ce25a000000000000000000000000000000000000000000000000000000000000000a");
	assert_x(toHex(abi1.Pack("getuint", 100)) == "de3d03930000000000000000000000000000000000000000000000000000000000000064");
	assert_x(toHex(abi1.Pack("getint8", 100)) == "bd3079420000000000000000000000000000000000000000000000000000000000000064");
	assert_x(toHex(abi1.Pack("getint40", 100)) == "f8699cb90000000000000000000000000000000000000000000000000000000000000064");
	assert_x(toHex(abi1.Pack("getint64", 100)) == "8197cdad0000000000000000000000000000000000000000000000000000000000000064");
	assert_x(toHex(abi1.Pack("getint128", 100)) == "ce6e3ebf0000000000000000000000000000000000000000000000000000000000000064");

	///2 test addresss and int,bool
	const std::string ABI2 = "[{\"inputs\":[{\"internalType\":\"address\",\"name\":\"\",\"type\":\"address\"}],\"name\":\"addressintstores\",\"outputs\":[{\"internalType\":\"int256\",\"name\":\"\",\"type\":\"int256\"}],\"stateMutability\":\"view\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"internalType\":\"address\",\"name\":\"a\",\"type\":\"address\"},{\"internalType\":\"int256\",\"name\":\"value\",\"type\":\"int256\"},{\"internalType\":\"int256\",\"name\":\"b\",\"type\":\"int256\"},{\"internalType\":\"int256\",\"name\":\"c\",\"type\":\"int256\"},{\"internalType\":\"bool\",\"name\":\"add\",\"type\":\"bool\"}],\"name\":\"setaddressint\",\"outputs\":[],\"stateMutability\":\"nonpayable\",\"type\":\"function\"},{\"inputs\":[{\"internalType\":\"address\",\"name\":\"a\",\"type\":\"address\"}],\"name\":\"getaddressint\",\"outputs\":[{\"internalType\":\"int256\",\"name\":\"\",\"type\":\"int256\"}],\"stateMutability\":\"view\",\"type\":\"function\",\"constant\":true}]";
	ABI abi2 = dev::JSON(ABI2);
	dev::h160 addr("0xD2b53c6dcF4EB754DE108ec0420EE7EcfC1223B3");
	assert_x(toHex(abi2.Pack("setaddressint", addr, 1, 2, 3, true)) == "4310e3bc000000000000000000000000d2b53c6dcf4eb754de108ec0420ee7ecfc1223b30000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000000000000000000000030000000000000000000000000000000000000000000000000000000000000001");
	assert_x(toHex(abi2.Pack("getaddressint", addr)) == "4ea4924a000000000000000000000000d2b53c6dcf4eb754de108ec0420ee7ecfc1223b3");

	/// 3 test bool and int slice
	const std::string ABI3 = "[{\"inputs\":[{\"internalType\":\"string\",\"name\":\"\",\"type\":\"string\"}],\"name\":\"stringintstores\",\"outputs\":[{\"internalType\":\"int256\",\"name\":\"\",\"type\":\"int256\"}],\"stateMutability\":\"view\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"internalType\":\"string\",\"name\":\"a\",\"type\":\"string\"},{\"internalType\":\"int256\",\"name\":\"value\",\"type\":\"int256\"}],\"name\":\"setstringint\",\"outputs\":[],\"stateMutability\":\"nonpayable\",\"type\":\"function\"},{\"inputs\":[{\"internalType\":\"string\",\"name\":\"a\",\"type\":\"string\"}],\"name\":\"getstringint\",\"outputs\":[{\"internalType\":\"int256\",\"name\":\"\",\"type\":\"int256\"}],\"stateMutability\":\"view\",\"type\":\"function\",\"constant\":true}]";
	ABI abi3 = dev::JSON(ABI3);
	assert_x(toHex(abi3.Pack("setstringint", "aa", 9)) == "2710add20000000000000000000000000000000000000000000000000000000000000040000000000000000000000000000000000000000000000000000000000000000900000000000000000000000000000000000000000000000000000000000000026161000000000000000000000000000000000000000000000000000000000000");
	assert_x(toHex(abi3.Pack("getstringint", "aa")) == "0033b628000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000000026161000000000000000000000000000000000000000000000000000000000000");

	/// 4 test bool and int slice
	const std::string ABI4 = "[{\"inputs\":[{\"internalType\":\"bool[]\",\"name\":\"a\",\"type\":\"bool[]\"},{\"internalType\":\"uint256[]\",\"name\":\"b\",\"type\":\"uint256[]\"}],\"name\":\"getintslice\",\"outputs\":[],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"internalType\":\"bool[5]\",\"name\":\"a\",\"type\":\"bool[5]\"},{\"internalType\":\"uint256[2]\",\"name\":\"b\",\"type\":\"uint256[2]\"}],\"name\":\"getintslice2\",\"outputs\":[],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true}]";
	ABI abi4 = dev::JSON(ABI4);
	std::vector<bool> arrbool4 = { true,false,false };
	std::vector<int> arrint4 = { 5, 6, 7 };
	assert_x(toHex(abi4.Pack("getintslice", arrbool4, arrint4)) == "549001d1000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000c000000000000000000000000000000000000000000000000000000000000000030000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003000000000000000000000000000000000000000000000000000000000000000500000000000000000000000000000000000000000000000000000000000000060000000000000000000000000000000000000000000000000000000000000007");

	const std::string ABI4_1 = "[{\"inputs\":[{\"internalType\":\"bool[5]\",\"name\":\"a\",\"type\":\"bool[5]\"},{\"internalType\":\"uint256[2]\",\"name\":\"b\",\"type\":\"uint256[2]\"}],\"name\":\"getintslice2\",\"outputs\":[],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true}]";
	ABI abi4_1 = dev::JSON(ABI4_1);
	std::vector<bool> arrbool4_1 = { true,false,false,true,true };
	std::vector<int> arrint4_1 = { 5, 6 };
	assert_x(toHex(abi4_1.Pack("getintslice2", arrbool4_1, arrint4_1)) == "277688d00000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000050000000000000000000000000000000000000000000000000000000000000006");


	///5 test string slice
	const std::string ABI5 = "[{\"inputs\":[{\"internalType\":\"string[]\",\"name\":\"a\",\"type\":\"string[]\"},{\"internalType\":\"uint256[]\",\"name\":\"b\",\"type\":\"uint256[]\"}],\"name\":\"getstringslice\",\"outputs\":[],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true}]";
	ABI abi5 = dev::JSON(ABI5);
	std::vector<std::string> arrstring5 = { "a","b" };
	std::vector<int> arrint5 = { 5, 6, 7 };
	assert_x(toHex(abi5.Pack("getstringslice", arrstring5, arrint5)) == "82a4c6ea0000000000000000000000000000000000000000000000000000000000000040000000000000000000000000000000000000000000000000000000000000012000000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000000000000000040000000000000000000000000000000000000000000000000000000000000008000000000000000000000000000000000000000000000000000000000000000016100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000162000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003000000000000000000000000000000000000000000000000000000000000000500000000000000000000000000000000000000000000000000000000000000060000000000000000000000000000000000000000000000000000000000000007");
	std::vector<std::string> arrstring5_0 = { "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz","b" };
	//std::cout << toHex(abi5.Pack("getstringslice", arrstring5_0, arrint5)) << std::endl;
	assert_x(toHex(abi5.Pack("getstringslice", arrstring5_0, arrint5)) == "82a4c6ea000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000001600000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000c0000000000000000000000000000000000000000000000000000000000000004e6162636465666768696a6b6c6d6e6f707172737475767778797a6162636465666768696a6b6c6d6e6f707172737475767778797a6162636465666768696a6b6c6d6e6f707172737475767778797a000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000162000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003000000000000000000000000000000000000000000000000000000000000000500000000000000000000000000000000000000000000000000000000000000060000000000000000000000000000000000000000000000000000000000000007");

	const std::string ABI5_1 = "[{\"inputs\":[{\"internalType\":\"string[2]\",\"name\":\"a\",\"type\":\"string[2]\"},{\"internalType\":\"uint256[3]\",\"name\":\"b\",\"type\":\"uint256[3]\"}],\"name\":\"getstringslice2\",\"outputs\":[],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true}]";
	ABI abi5_1 = dev::JSON(ABI5_1);
	std::vector<std::string> arrstring5_1 = { "a","b" };
	std::vector<int> arrint5_1 = { 5, 6, 7 };
	assert_x(toHex(abi5_1.Pack("getstringslice2", arrstring5_1, arrint5_1)) == "ff1392100000000000000000000000000000000000000000000000000000000000000080000000000000000000000000000000000000000000000000000000000000000500000000000000000000000000000000000000000000000000000000000000060000000000000000000000000000000000000000000000000000000000000007000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000800000000000000000000000000000000000000000000000000000000000000001610000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000016200000000000000000000000000000000000000000000000000000000000000");
	std::vector<std::string> arrstring5_1_1 = { "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz","b" };
	//std::cout << toHex(abi5_1.Pack("getstringslice2", arrstring5_1_1, arrint5_1)) << std::endl;
	assert_x(toHex(abi5_1.Pack("getstringslice2", arrstring5_1_1, arrint5_1)) == "ff1392100000000000000000000000000000000000000000000000000000000000000080000000000000000000000000000000000000000000000000000000000000000500000000000000000000000000000000000000000000000000000000000000060000000000000000000000000000000000000000000000000000000000000007000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000c0000000000000000000000000000000000000000000000000000000000000004e6162636465666768696a6b6c6d6e6f707172737475767778797a6162636465666768696a6b6c6d6e6f707172737475767778797a6162636465666768696a6b6c6d6e6f707172737475767778797a00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000016200000000000000000000000000000000000000000000000000000000000000");

	///6 test bytes slice
	const std::string ABI6 = "[{\"inputs\":[{\"internalType\":\"bytes\",\"name\":\"a\",\"type\":\"bytes\"},{\"internalType\":\"uint256\",\"name\":\"b\",\"type\":\"uint256\"}],\"name\":\"getbytesslice\",\"outputs\":[],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"internalType\":\"bytes[]\",\"name\":\"a\",\"type\":\"bytes[]\"},{\"internalType\":\"uint256\",\"name\":\"b\",\"type\":\"uint256\"}],\"name\":\"getbytesslice2\",\"outputs\":[],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"internalType\":\"bytes[2]\",\"name\":\"a\",\"type\":\"bytes[2]\"},{\"internalType\":\"uint256\",\"name\":\"b\",\"type\":\"uint256\"}],\"name\":\"getbytesslice3\",\"outputs\":[],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"internalType\":\"bytes3\",\"name\":\"a\",\"type\":\"bytes3\"},{\"internalType\":\"uint256\",\"name\":\"b\",\"type\":\"uint256\"}],\"name\":\"getbytesslice4\",\"outputs\":[],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"internalType\":\"bytes3[]\",\"name\":\"a\",\"type\":\"bytes3[]\"},{\"internalType\":\"uint256\",\"name\":\"b\",\"type\":\"uint256\"}],\"name\":\"getbytesslice5\",\"outputs\":[],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"internalType\":\"bytes3[2]\",\"name\":\"a\",\"type\":\"bytes3[2]\"},{\"internalType\":\"uint256\",\"name\":\"b\",\"type\":\"uint256\"}],\"name\":\"getbytesslice6\",\"outputs\":[],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true}]";
	ABI abi6 = dev::JSON(ABI6);
	dev::bytes b6 = dev::fromHex("0x0102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c");
	//std::cout << toHex(abi6.Pack("getbytesslice", b6, 5)) << std::endl;
	assert_x(toHex(abi6.Pack("getbytesslice", b6, 5)) == "dc7469620000000000000000000000000000000000000000000000000000000000000040000000000000000000000000000000000000000000000000000000000000000500000000000000000000000000000000000000000000000000000000000000210102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c00000000000000000000000000000000000000000000000000000000000000");

	std::vector<dev::bytes> b6_1;
	b6_1.push_back(dev::fromHex("0x0102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c"));
	b6_1.push_back(dev::fromHex("0x5c"));
	//std::cout << toHex(abi6.Pack("getbytesslice2", b6_1, 5)) << std::endl;
	assert_x(toHex(abi6.Pack("getbytesslice2", b6_1, 5)) == "418c6dab000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000050000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000a000000000000000000000000000000000000000000000000000000000000000210102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000015c00000000000000000000000000000000000000000000000000000000000000");

	//std::cout << toHex(abi6.Pack("getbytesslice3", b6_1, 5)) << std::endl;
	assert_x(toHex(abi6.Pack("getbytesslice3", b6_1, 5)) == "f3a6ea1500000000000000000000000000000000000000000000000000000000000000400000000000000000000000000000000000000000000000000000000000000005000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000a000000000000000000000000000000000000000000000000000000000000000210102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000015c00000000000000000000000000000000000000000000000000000000000000");

	dev::bytes b6_4 = dev::fromHex("0x010203");
	//std::cout << toHex(abi6.Pack("getbytesslice4", b6_4, 5)) << std::endl;
	assert_x(toHex(abi6.Pack("getbytesslice4", b6_4, 5)) == "0dba89fc01020300000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000005");

	//const std::string ABI6 = "[{\"inputs\":[{\"internalType\":\"bytes3\",\"name\":\"a\",\"type\":\"bytes3\"},{\"internalType\":\"uint256\",\"name\":\"b\",\"type\":\"uint256\"}],\"name\":\"getbytesslice4\",\"outputs\":[],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true}]";
	//auto abi6 = dev::NewBoundContract(dev::h160(), dev::JSON(ABI6));
	std::vector<dev::bytes> b6_5;
	b6_5.push_back(dev::fromHex("0x010203"));
	b6_5.push_back(dev::fromHex("0x040506"));
	//std::cout << toHex(abi6.Pack("getbytesslice5", b6_5, 5)) << std::endl;
	assert_x(toHex(abi6.Pack("getbytesslice5", b6_5, 5)) == "0f9b9d8400000000000000000000000000000000000000000000000000000000000000400000000000000000000000000000000000000000000000000000000000000005000000000000000000000000000000000000000000000000000000000000000201020300000000000000000000000000000000000000000000000000000000000405060000000000000000000000000000000000000000000000000000000000");

	std::vector<dev::bytes> b6_6;
	b6_6.push_back(dev::fromHex("0x010203"));
	b6_6.push_back(dev::fromHex("0x040506"));
	//std::cout << toHex(abi6.Pack("getbytesslice6", b6_6, 5)) << std::endl;
	assert_x(toHex(abi6.Pack("getbytesslice6", b6_6, 5)) == "d00fc06a010203000000000000000000000000000000000000000000000000000000000004050600000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000005");

	///7 test trup
	///gettrup1
	//const std::string ABI7 = "[{\"inputs\":[{\"components\":[{\"internalType\":\"int256\",\"name\":\"value1\",\"type\":\"int256\"},{\"internalType\":\"int128\",\"name\":\"value2\",\"type\":\"int128\"},{\"internalType\":\"address\",\"name\":\"value3\",\"type\":\"address\"},{\"internalType\":\"string\",\"name\":\"value4\",\"type\":\"string\"},{\"internalType\":\"bytes\",\"name\":\"value5\",\"type\":\"bytes\"}],\"internalType\":\"struct Store.st1\",\"name\":\"v1\",\"type\":\"tuple\"},{\"internalType\":\"int256\",\"name\":\"valueint\",\"type\":\"int256\"}],\"name\":\"gettrup1\",\"outputs\":[],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"components\":[{\"internalType\":\"int256[]\",\"name\":\"valueint1\",\"type\":\"int256[]\"},{\"internalType\":\"int256[2]\",\"name\":\"valueint2\",\"type\":\"int256[2]\"},{\"internalType\":\"address[]\",\"name\":\"valueaddress1\",\"type\":\"address[]\"},{\"internalType\":\"address[2]\",\"name\":\"valueaddress2\",\"type\":\"address[2]\"},{\"internalType\":\"string[]\",\"name\":\"valuestring1\",\"type\":\"string[]\"},{\"internalType\":\"string[2]\",\"name\":\"valuestring2\",\"type\":\"string[2]\"},{\"internalType\":\"bytes[]\",\"name\":\"valuebytes1\",\"type\":\"bytes[]\"},{\"internalType\":\"bytes[2]\",\"name\":\"valuebytes2\",\"type\":\"bytes[2]\"},{\"internalType\":\"bytes3[]\",\"name\":\"valuebytes31\",\"type\":\"bytes3[]\"},{\"internalType\":\"bytes3[2]\",\"name\":\"valuebytes32\",\"type\":\"bytes3[2]\"}],\"internalType\":\"struct Store.st2\",\"name\":\"v1\",\"type\":\"tuple\"},{\"internalType\":\"int256\",\"name\":\"valueint\",\"type\":\"int256\"}],\"name\":\"gettrup2\",\"outputs\":[],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"components\":[{\"internalType\":\"int256[]\",\"name\":\"valueint1\",\"type\":\"int256[]\"},{\"internalType\":\"string\",\"name\":\"value4\",\"type\":\"string\"}],\"internalType\":\"struct Store.st3[]\",\"name\":\"v1\",\"type\":\"tuple[]\"},{\"internalType\":\"int256\",\"name\":\"valueint\",\"type\":\"int256\"}],\"name\":\"gettrup3\",\"outputs\":[],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true}]";
	const std::string ABI7 = "[{\"inputs\":[{\"components\":[{\"internalType\":\"int256\",\"name\":\"value1\",\"type\":\"int256\"},{\"internalType\":\"int128\",\"name\":\"value2\",\"type\":\"int128\"},{\"internalType\":\"address\",\"name\":\"value3\",\"type\":\"address\"},{\"internalType\":\"string\",\"name\":\"value4\",\"type\":\"string\"},{\"internalType\":\"bytes\",\"name\":\"value5\",\"type\":\"bytes\"}],\"internalType\":\"struct Store.st1\",\"name\":\"v1\",\"type\":\"tuple\"},{\"internalType\":\"int256\",\"name\":\"valueint\",\"type\":\"int256\"}],\"name\":\"gettrup1\",\"outputs\":[],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"components\":[{\"internalType\":\"int256[]\",\"name\":\"valueint1\",\"type\":\"int256[]\"},{\"internalType\":\"int256[2]\",\"name\":\"valueint2\",\"type\":\"int256[2]\"},{\"internalType\":\"address[]\",\"name\":\"valueaddress1\",\"type\":\"address[]\"},{\"internalType\":\"address[2]\",\"name\":\"valueaddress2\",\"type\":\"address[2]\"},{\"internalType\":\"string[]\",\"name\":\"valuestring1\",\"type\":\"string[]\"},{\"internalType\":\"string[2]\",\"name\":\"valuestring2\",\"type\":\"string[2]\"},{\"internalType\":\"bytes[]\",\"name\":\"valuebytes1\",\"type\":\"bytes[]\"},{\"internalType\":\"bytes[2]\",\"name\":\"valuebytes2\",\"type\":\"bytes[2]\"},{\"internalType\":\"bytes3[]\",\"name\":\"valuebytes31\",\"type\":\"bytes3[]\"},{\"internalType\":\"bytes3[2]\",\"name\":\"valuebytes32\",\"type\":\"bytes3[2]\"}],\"internalType\":\"struct Store.st2\",\"name\":\"v1\",\"type\":\"tuple\"},{\"internalType\":\"int256\",\"name\":\"valueint\",\"type\":\"int256\"}],\"name\":\"gettrup2\",\"outputs\":[],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"components\":[{\"internalType\":\"int256[]\",\"name\":\"valueint1\",\"type\":\"int256[]\"},{\"internalType\":\"string\",\"name\":\"value4\",\"type\":\"string\"}],\"internalType\":\"struct Store.st3[]\",\"name\":\"v1\",\"type\":\"tuple[]\"},{\"internalType\":\"int256\",\"name\":\"valueint\",\"type\":\"int256\"}],\"name\":\"gettrup3\",\"outputs\":[],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true},{\"inputs\":[{\"components\":[{\"internalType\":\"int256[]\",\"name\":\"valueint1\",\"type\":\"int256[]\"},{\"internalType\":\"string\",\"name\":\"value4\",\"type\":\"string\"}],\"internalType\":\"struct Store.st3[]\",\"name\":\"v1\",\"type\":\"tuple[]\"},{\"components\":[{\"internalType\":\"int256[]\",\"name\":\"valueint1\",\"type\":\"int256[]\"},{\"internalType\":\"int256[2]\",\"name\":\"valueint2\",\"type\":\"int256[2]\"},{\"internalType\":\"address[]\",\"name\":\"valueaddress1\",\"type\":\"address[]\"},{\"internalType\":\"address[2]\",\"name\":\"valueaddress2\",\"type\":\"address[2]\"},{\"internalType\":\"string[]\",\"name\":\"valuestring1\",\"type\":\"string[]\"},{\"internalType\":\"string[2]\",\"name\":\"valuestring2\",\"type\":\"string[2]\"},{\"internalType\":\"bytes[]\",\"name\":\"valuebytes1\",\"type\":\"bytes[]\"},{\"internalType\":\"bytes[2]\",\"name\":\"valuebytes2\",\"type\":\"bytes[2]\"},{\"internalType\":\"bytes3[]\",\"name\":\"valuebytes31\",\"type\":\"bytes3[]\"},{\"internalType\":\"bytes3[2]\",\"name\":\"valuebytes32\",\"type\":\"bytes3[2]\"}],\"internalType\":\"struct Store.st2\",\"name\":\"v2\",\"type\":\"tuple\"}],\"name\":\"gettrup4\",\"outputs\":[],\"stateMutability\":\"pure\",\"type\":\"function\",\"constant\":true}]";
	//auto contract7 = dev::NewBoundContract(dev::h160(), dev::JSON(ABI7));
	ABI abi7 = dev::JSON(ABI7);
	dev::h160 addr7("0xD2b53c6dcF4EB754DE108ec0420EE7EcfC1223B3");
	dev::bytes bytes7 = dev::fromHex("0x0102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c");
	auto tup7_1 = boost::make_tuple(100, 100, addr7, "abcdefg", bytes7);
	//std::cout << toHex(abi7.Pack("gettrup1", tup7_1,5)) << std::endl;
	assert_x(toHex(abi7.Pack("gettrup1", tup7_1, 5)) == "cdbb2f680000000000000000000000000000000000000000000000000000000000000040000000000000000000000000000000000000000000000000000000000000000500000000000000000000000000000000000000000000000000000000000000640000000000000000000000000000000000000000000000000000000000000064000000000000000000000000d2b53c6dcf4eb754de108ec0420ee7ecfc1223b300000000000000000000000000000000000000000000000000000000000000a000000000000000000000000000000000000000000000000000000000000000e00000000000000000000000000000000000000000000000000000000000000007616263646566670000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000210102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c00000000000000000000000000000000000000000000000000000000000000");

	///gettrup2
	std::vector<int> arrint7_1 = { 1, 2, 3 };
	std::vector<int> arrint7_2 = { 4, 5 };
	std::vector<dev::h160> arraddress7_1 = { dev::h160("0xD2b53c6dcF4EB754DE108ec0420EE7EcfC1223B3"),dev::h160("0x49a1b41e8ccb704f5c069ef89b08cd33f764e9b3"),dev::h160("0xf0821dc4ba9419b865aa412170377ca3b44cdb58") };
	std::vector<dev::h160> arraddress7_2 = { dev::h160("0x329e6b5b8e59fc73d892958b2ff6a89474e3d067"),dev::h160("0x827cce78dc6ec7051f2d7bb9e7adaefba7ca3248") };
	std::vector<std::string> arrstring7_1 = { "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz", "abc", "defgh" };
	std::vector<std::string> arrstring7_2 = { "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz", "aaa" };
	std::vector<dev::bytes> arrbytes7_1 = { dev::fromHex("0x0102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c"),dev::fromHex("0x5c"),dev::fromHex("0x5d5e5f") };
	std::vector<dev::bytes> arrbytes7_2 = { dev::fromHex("0x0102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c"),dev::fromHex("0x6a6b6c") };
	std::vector<dev::bytes> arrbytes37_1 = { dev::fromHex("0x010203"),dev::fromHex("0x040506"), dev::fromHex("0x0a0b0c") };
	std::vector<dev::bytes> arrbytes37_2 = { dev::fromHex("0x212223"),dev::fromHex("0x242526") };
	auto tup7_2 = boost::make_tuple(arrint7_1, arrint7_2, arraddress7_1, arraddress7_2, arrstring7_1, arrstring7_2, arrbytes7_1, arrbytes7_2, arrbytes37_1, arrbytes37_2);
	//std::cout << toHex(abi7.Pack("gettrup2", tup7_2, 5)) << std::endl;
	assert_x(toHex(abi7.Pack("gettrup2", tup7_2, 5)) == "150230d00000000000000000000000000000000000000000000000000000000000000040000000000000000000000000000000000000000000000000000000000000000500000000000000000000000000000000000000000000000000000000000001a0000000000000000000000000000000000000000000000000000000000000000400000000000000000000000000000000000000000000000000000000000000050000000000000000000000000000000000000000000000000000000000000220000000000000000000000000329e6b5b8e59fc73d892958b2ff6a89474e3d067000000000000000000000000827cce78dc6ec7051f2d7bb9e7adaefba7ca324800000000000000000000000000000000000000000000000000000000000002a000000000000000000000000000000000000000000000000000000000000004200000000000000000000000000000000000000000000000000000000000000520000000000000000000000000000000000000000000000000000000000000068000000000000000000000000000000000000000000000000000000000000007602122230000000000000000000000000000000000000000000000000000000000242526000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000030000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000000000000000000000030000000000000000000000000000000000000000000000000000000000000003000000000000000000000000d2b53c6dcf4eb754de108ec0420ee7ecfc1223b300000000000000000000000049a1b41e8ccb704f5c069ef89b08cd33f764e9b3000000000000000000000000f0821dc4ba9419b865aa412170377ca3b44cdb580000000000000000000000000000000000000000000000000000000000000003000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000e00000000000000000000000000000000000000000000000000000000000000120000000000000000000000000000000000000000000000000000000000000004e6162636465666768696a6b6c6d6e6f707172737475767778797a6162636465666768696a6b6c6d6e6f707172737475767778797a6162636465666768696a6b6c6d6e6f707172737475767778797a0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003616263000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000056465666768000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000c0000000000000000000000000000000000000000000000000000000000000004e6162636465666768696a6b6c6d6e6f707172737475767778797a6162636465666768696a6b6c6d6e6f707172737475767778797a6162636465666768696a6b6c6d6e6f707172737475767778797a000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000361616100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000c0000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000210102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000015c0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000035d5e5f0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000a000000000000000000000000000000000000000000000000000000000000000210102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000036a6b6c00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003010203000000000000000000000000000000000000000000000000000000000004050600000000000000000000000000000000000000000000000000000000000a0b0c0000000000000000000000000000000000000000000000000000000000");

	///gettrup3
	std::vector<int> arrint7_3_1 = { 1, 2, 3 };
	std::string string7_3_1 = "abc";
	auto tup7_3_1 = boost::make_tuple(arrint7_3_1, string7_3_1);
	std::vector<int> arrint7_3_2 = { 4, 5 };
	std::string string7_3_2 = "def";
	auto tup7_3_2 = boost::make_tuple(arrint7_3_2, string7_3_2);
	std::vector<boost::tuple<std::vector<int>, std::string>> tup7_3;
	tup7_3.push_back(tup7_3_1);
	tup7_3.push_back(tup7_3_2);
	//std::cout << toHex(abi7.Pack("gettrup3", tup7_3, 5)) << std::endl;
	assert_x(toHex(abi7.Pack("gettrup3", tup7_3, 5)) == "076a363800000000000000000000000000000000000000000000000000000000000000400000000000000000000000000000000000000000000000000000000000000005000000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000000000000000000000400000000000000000000000000000000000000000000000000000000000000140000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000c0000000000000000000000000000000000000000000000000000000000000000300000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000000300000000000000000000000000000000000000000000000000000000000000036162630000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000a000000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000500000000000000000000000000000000000000000000000000000000000000036465660000000000000000000000000000000000000000000000000000000000");

	///gettrup4
	//std::cout << toHex(abi7.Pack("gettrup4", tup7_3, tup7_2)) << std::endl;
	assert_x(toHex(abi7.Pack("gettrup4", tup7_3, tup7_2)) == "eb109c0a00000000000000000000000000000000000000000000000000000000000000400000000000000000000000000000000000000000000000000000000000000280000000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000000000000000000000400000000000000000000000000000000000000000000000000000000000000140000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000c0000000000000000000000000000000000000000000000000000000000000000300000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000000300000000000000000000000000000000000000000000000000000000000000036162630000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000000400000000000000000000000000000000000000000000000000000000000000050000000000000000000000000000000000000000000000000000000000000003646566000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001a0000000000000000000000000000000000000000000000000000000000000000400000000000000000000000000000000000000000000000000000000000000050000000000000000000000000000000000000000000000000000000000000220000000000000000000000000329e6b5b8e59fc73d892958b2ff6a89474e3d067000000000000000000000000827cce78dc6ec7051f2d7bb9e7adaefba7ca324800000000000000000000000000000000000000000000000000000000000002a000000000000000000000000000000000000000000000000000000000000004200000000000000000000000000000000000000000000000000000000000000520000000000000000000000000000000000000000000000000000000000000068000000000000000000000000000000000000000000000000000000000000007602122230000000000000000000000000000000000000000000000000000000000242526000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000030000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000000000000000000000030000000000000000000000000000000000000000000000000000000000000003000000000000000000000000d2b53c6dcf4eb754de108ec0420ee7ecfc1223b300000000000000000000000049a1b41e8ccb704f5c069ef89b08cd33f764e9b3000000000000000000000000f0821dc4ba9419b865aa412170377ca3b44cdb580000000000000000000000000000000000000000000000000000000000000003000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000e00000000000000000000000000000000000000000000000000000000000000120000000000000000000000000000000000000000000000000000000000000004e6162636465666768696a6b6c6d6e6f707172737475767778797a6162636465666768696a6b6c6d6e6f707172737475767778797a6162636465666768696a6b6c6d6e6f707172737475767778797a0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003616263000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000056465666768000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000c0000000000000000000000000000000000000000000000000000000000000004e6162636465666768696a6b6c6d6e6f707172737475767778797a6162636465666768696a6b6c6d6e6f707172737475767778797a6162636465666768696a6b6c6d6e6f707172737475767778797a000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000361616100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000c0000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000210102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000015c0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000035d5e5f0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000a000000000000000000000000000000000000000000000000000000000000000210102030405060708090a0b0c0d0e0f1112131415161718191a1b1c1d1e1f3a3b3c0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000036a6b6c00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003010203000000000000000000000000000000000000000000000000000000000004050600000000000000000000000000000000000000000000000000000000000a0b0c0000000000000000000000000000000000000000000000000000000000");

}
