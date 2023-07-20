#pragma once

#include <libdevcore/Exceptions.h>
#include <mcp/common/Exceptions.h>

namespace mcp
{
	struct RpcEthException : virtual Exception
	{
		const char* what() const noexcept override { return "OK"; }
		const int virtual code() const noexcept { return 0; }
		const void virtual toJson(json & j_response) const noexcept {
			if(code()<0){
				// if(j_response.count("code") == 1){
				// 	j_response.erase("code");
				// }
				// if(j_response.count("msg") == 1){
				// 	j_response.erase("msg");
				// }
				// if(j_response.count("result") == 1){
				// 	j_response.erase("result");
				// }
				json error;
				error["code"] = code();
				error["message"] = what();
				j_response["error"] = error;
			}
		}
	};

	struct RpcEthException_No_Result : virtual Exception
	{
		const char* what() const noexcept override { return "OK"; }
		const int virtual code() const noexcept { return 0; }
		const void virtual toJson(json & j_response) const noexcept {
			if(code()<0){
				// if(j_response.count("code") == 1){
				// 	j_response.erase("code");
				// }
				// if(j_response.count("msg") == 1){
				// 	j_response.erase("msg");
				// }
				// if(j_response.count("result") == 1){
				// 	j_response.erase("result");
				// }
				json error;
				error["code"] = code();
				error["message"] = what();
				j_response["error"] = error;
			}
		}
	};

	// struct RpcEthException : RpcException
	// {
	// 	const void toJson(json & j_response) const noexcept override {
	// 		json error;
	// 		error["code"] = code();
	// 		error["message"] = what();
	// 		j_response["error"] = error;
	// 	}
	// };


// #define RPC_ERROR_EXCEPTION(X, C, M)  \
//     struct X : virtual RpcException \
//     {                            \
// 		const char* what() const noexcept override { return M; } \
// 		const int code() const noexcept override { return C; } \
//     }

#define NEW_RPC_ETH_ERROR_EXCEPTION(X, C)  \
    struct X : virtual RpcEthException \
    {                            \
		X(const char* m) : message(m) {} \
		const char* what() const noexcept override { return message; }\
		const int code() const noexcept override { return C; } \
	private: \
        const char* message; \
    }

#define NEW_RPC_ETH_ERROR_NORESULT(X, C)  \
    struct X : virtual RpcEthException_No_Result \
    {                            \
		X(const char* m) : message(m) {} \
		const char* what() const noexcept override { return message; }\
		const int code() const noexcept override { return C; } \
	private: \
        const char* message; \
    }

// #define NEW_RPC_ERROR_EXCEPTION(X, C)  \
//     struct X : virtual RpcException \
//     {                            \
// 		X(const char* m) : message(m) {} \
// 		const char* what() const noexcept override { return message; }\
// 		const int code() const noexcept override { return C; } \
// 	private: \
//         const char* message; \
//     }

#define RPC_ETH_ERROR_EXCEPTION(X, C, M)  \
    struct X : virtual RpcEthException \
    {                            \
		const char* what() const noexcept override { return M; } \
		const int code() const noexcept override { return C; } \
    }
	
	/***************************************************************************************************************************/
	//new rpc exception structure definition:
	NEW_RPC_ETH_ERROR_EXCEPTION(NEW_RPC_Eth_Error_ServerProcedureSpecificationNotFound,-32000);//not used yet
	NEW_RPC_ETH_ERROR_EXCEPTION(NEW_RPC_Eth_Error_ClientInvalidResponse,-32001);//not used yet
	NEW_RPC_ETH_ERROR_EXCEPTION(NEW_RPC_Eth_Error_TimeOut,-32002);//not used yet
	NEW_RPC_ETH_ERROR_EXCEPTION(NEW_RPC_Eth_Error_TransactionRejected,-32003);//not used yet
	NEW_RPC_ETH_ERROR_EXCEPTION(NEW_RPC_Eth_Error_TooLargeSearchRange,-32005);
	NEW_RPC_ETH_ERROR_EXCEPTION(NEW_RPC_Eth_Error_InvalidRequest,-32600);
	NEW_RPC_ETH_ERROR_EXCEPTION(NEW_RPC_Eth_Error_MethodNotFound,-32601);
	NEW_RPC_ETH_ERROR_EXCEPTION(NEW_RPC_Eth_Error_InvalidParams,-32602);
	NEW_RPC_ETH_ERROR_EXCEPTION(NEW_RPC_Eth_Error_InternalError,-32603);
	NEW_RPC_ETH_ERROR_EXCEPTION(NEW_RPC_Eth_Error_JsonParseError,-32700);
	NEW_RPC_ETH_ERROR_NORESULT(NEW_RPC_Eth_Error_InvalidArgument,-32602);

	template<typename Base, typename T>
	inline bool instanceof(const T *ptr) {
		return dynamic_cast<const Base*>(ptr) != nullptr;
	}

	inline void toRpcExceptionJson(Exception const & e, json & j_response) {
		if (instanceof<dev::GasPriceTooLow>(&e) ||
			instanceof<dev::BlockGasLimitReached>(&e) ||
			instanceof<dev::GasPriceTooLow>(&e)) {
			NEW_RPC_Eth_Error_InvalidParams("Invalid Gas").toJson(j_response);
		}
		else if (instanceof<dev::NotEnoughCash>(&e)) {
			NEW_RPC_Eth_Error_ServerProcedureSpecificationNotFound("Insufficient Balance").toJson(j_response);
		}
		else if (instanceof<dev::PendingTransactionAlreadyExists>(&e)) {
			NEW_RPC_Eth_Error_InternalError("Pending Transaction Already Exists").toJson(j_response);
		}
		else if (instanceof<dev::TransactionAlreadyInChain>(&e)) {
			NEW_RPC_Eth_Error_InternalError("Transaction already exists in chain").toJson(j_response);
		}
		else if (instanceof<dev::UnknownTransactionValidationError>(&e)) {
			NEW_RPC_Eth_Error_InternalError("Validation Error").toJson(j_response);
		}
		else if (instanceof<dev::AccountLocked>(&e)) {
			NEW_RPC_Eth_Error_InvalidParams("Account Locked").toJson(j_response);
		}
		else if (instanceof<dev::UnknownAccount>(&e)) {
			NEW_RPC_Eth_Error_InvalidParams("Invalid Account").toJson(j_response);
		}
		else {
			NEW_RPC_Eth_Error_InvalidParams("Invalid Params").toJson(j_response);
		}
	}

	inline void toRpcExceptionEthJson(Exception const & e, json & j_response) {
		if (instanceof<dev::OutOfGasIntrinsic>(&e)) {
			NEW_RPC_Eth_Error_InvalidParams("Out of Gas Instrinsic").toJson(j_response);
		}
		else if (instanceof<dev::OutOfGasPriceIntrinsic>(&e)) {
			NEW_RPC_Eth_Error_InvalidParams("Out of Gas Price Instrinsic").toJson(j_response);
		}
		else if (instanceof<dev::BlockGasLimitReached>(&e)) {
			NEW_RPC_Eth_Error_InvalidParams("Block Gas Limit Reached").toJson(j_response);
		}
		else if (instanceof<dev::GasPriceTooLow>(&e)) {
			NEW_RPC_Eth_Error_InvalidParams("Gas Price Too Low").toJson(j_response);
		}
		else if (instanceof<dev::ZeroSignatureTransaction>(&e)) {
			NEW_RPC_Eth_Error_InvalidParams("Zero Signature Transaction").toJson(j_response);
		}
		else if (instanceof<dev::InvalidNonce>(&e)) {
			NEW_RPC_Eth_Error_InvalidParams("Invalid Nounce").toJson(j_response);
		}
		else if (instanceof<dev::NotEnoughCash>(&e)) {
			NEW_RPC_Eth_Error_ServerProcedureSpecificationNotFound("Insufficient Balance").toJson(j_response);
		}
		else if (instanceof<dev::InvalidSignature>(&e)) {
			NEW_RPC_Eth_Error_InvalidParams("Invalid Signature").toJson(j_response);
		}
		else if (instanceof<dev::PendingTransactionAlreadyExists>(&e)) {//********
			NEW_RPC_Eth_Error_InternalError("Pending Transaction Already Exists").toJson(j_response);
		}
		else if (instanceof<dev::TransactionAlreadyInChain>(&e)) {//********
			NEW_RPC_Eth_Error_InternalError("Transaction already exists in chain").toJson(j_response);
		}
		else if (instanceof<dev::UnknownTransactionValidationError>(&e)) {//********
			NEW_RPC_Eth_Error_InternalError("Validation Error").toJson(j_response);
		}
		else if (instanceof<dev::AccountLocked>(&e)) {
			NEW_RPC_Eth_Error_InvalidParams("Locked Account").toJson(j_response);
		}
		else if (instanceof<dev::PendingTransactionTooMuch>(&e)) {//******
			NEW_RPC_Eth_Error_InternalError("Account's Pending transaction is full").toJson(j_response);
		}
		else if (instanceof<dev::UnknownAccount>(&e)) {
			NEW_RPC_Eth_Error_InvalidParams("Invalid Account").toJson(j_response);
		}
		else if (instanceof<dev::eth::ExtraDataTooBig>(&e)) {
			NEW_RPC_Eth_Error_InvalidParams("Data Too Large").toJson(j_response);
		}
		else if (instanceof<dev::eth::NotEnoughCash>(&e)) {
			NEW_RPC_Eth_Error_ServerProcedureSpecificationNotFound("Insufficient Balance").toJson(j_response);
		}
		else {
			NEW_RPC_Eth_Error_InvalidParams("Invalid Param").toJson(j_response);
		}
	}

	inline void toRpcExceptionEthInvalidArgument(std::exception const & e, json & j_response)
	{
		if (instanceof<dev::OutOfGasIntrinsic>(&e)) {
			NEW_RPC_Eth_Error_InvalidParams("Out of Gas Instrinsic").toJson(j_response);
		}
		else if(instanceof<std::invalid_argument>(&e)){
			NEW_RPC_Eth_Error_InvalidParams(e.what()).toJson(j_response);
		}
		else if(instanceof<dev::BadHexCharacter>(&e)){
			NEW_RPC_Eth_Error_InvalidParams("Invalid Argument: Bad Hex Character").toJson(j_response);
		}
		else if(instanceof<mcp::json::exception>(&e)){
			NEW_RPC_Eth_Error_InvalidParams(e.what()).toJson(j_response);
		}
		else{
			NEW_RPC_Eth_Error_InvalidParams("Unknown Error").toJson(j_response);
		}
	}
}
