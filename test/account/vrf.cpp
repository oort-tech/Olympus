#include <mcp/core/common.hpp>

void test_vrf()
{
	std::string msg = "f2052e88cb2b777773aa3abab2024cd9a2a305043584c0b18639dc303841c4fc";
	auto sec = dev::Secret("d79703a37d55fd5afc17fa4bf98047f9c6592559abe107d01fad13f8cdd0cd2a");
	secp256k1_pubkey rawPubkey = dev::toPublickey(sec);
	dev::Public _public = dev::toPublic(sec);
	dev::PublicCompressed _publicCompressed = dev::toPublicCompressed(_public);

	h256 output;
	h648 proof;

	if (secp256k1_vrf_prove(proof.data(), sec.data(), &rawPubkey, msg.data(), msg.size())) {
		std::cout << "[send_approve] secp256k1_vrf_prove ok" << std::endl;
		std::cout << "proof:" << proof.hex() << std::endl;
	}
	else {
		std::cout << "[send_approve] secp256k1_vrf_prove fail" << std::endl;
	}

	if (secp256k1_vrf_verify(output.data(), proof.data(), _publicCompressed.data(), msg.data(), msg.size())) {
		std::cout << "[send_approve] secp256k1_vrf_verify ok" << std::endl;
		std::cout << "out:" << output.hex() << std::endl;
	}
	else {
		std::cout << "[send_approve] secp256k1_vrf_verify fail" << std::endl;
	}
}