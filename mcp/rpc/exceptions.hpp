#pragma once

#include <libdevcore/Exceptions.h>
#include <mcp/common/Exceptions.h>
#include <boost/beast.hpp>

namespace mcp
{
	/***************************************************************************************************************************/
	///http exception structure definition:
	struct RpcHttpException : virtual Exception
	{
		const char* what() const noexcept override { return ""; }
		const boost::beast::http::status virtual status() const noexcept { return boost::beast::http::status::ok; }
	};

	///bad request.
	struct RPC_Http_Error_BadRequest : virtual RpcHttpException
	{
		RPC_Http_Error_BadRequest(const char* m) : message(m) {}
		const char* what() const noexcept override { return message; }
		const boost::beast::http::status status() const noexcept override { return boost::beast::http::status::bad_request; }
	private: 
		const char* message; 
	};

	///internal error.
	struct RPC_Http_Error_Internal_Server_Error : virtual RpcHttpException
	{
		RPC_Http_Error_Internal_Server_Error(const char* m) : message(m) {}
		const char* what() const noexcept override { return message; }
		const boost::beast::http::status status() const noexcept override { return boost::beast::http::status::internal_server_error; }
	private:
		const char* message;
	};

	///system exception
	struct RpcException : virtual Exception
	{
		const char* what() const noexcept override { return "OK"; }
		const int virtual code() const noexcept { return 0; }
		const void virtual toJson(json & j_response) const noexcept {
			json error;
			error["code"] = code();
			error["message"] = what();
			j_response["error"] = error;
		}
	};

#define RPC_ERROR_EXCEPTION(X, C)  \
    struct X : virtual RpcException \
    {                            \
		X(const char* m) : message(m) {} \
		const char* what() const noexcept override { return message.what(); }\
		const int code() const noexcept override { return C; } \
	private: \
        std::runtime_error message; \
    };
	
	/***************************************************************************************************************************/
	//rpc exception structure definition:
	RPC_ERROR_EXCEPTION(RPC_Error_ServerProcedureSpecificationNotFound,-32000);//not used yet
	RPC_ERROR_EXCEPTION(RPC_Error_ClientInvalidResponse,-32001);//not used yet
	RPC_ERROR_EXCEPTION(RPC_Error_TimeOut,-32002);//not used yet
	RPC_ERROR_EXCEPTION(RPC_Error_TransactionRejected,-32003);//not used yet
	RPC_ERROR_EXCEPTION(RPC_Error_TooLargeSearchRange,-32005);
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidRequest,-32600);
	RPC_ERROR_EXCEPTION(RPC_Error_MethodNotFound,-32601);
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidParams,-32602);
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidParams_No_Result,-32602);
	RPC_ERROR_EXCEPTION(RPC_Error_InternalError,-32603);
	RPC_ERROR_EXCEPTION(RPC_Error_JsonParseError,-32700);

	template<typename Base, typename T>
	inline bool instanceof(const T *ptr) {
		return dynamic_cast<const Base*>(ptr) != nullptr;
	}

	inline void toRpcExceptionEthJson(std::exception const & e, json & j_response) {
		if (instanceof<dev::OutOfGasIntrinsic>(&e)) {
			RPC_Error_InvalidParams("Out of Gas Instrinsic").toJson(j_response);
		}
		else if (instanceof<dev::OutOfGasPriceIntrinsic>(&e)) {
			RPC_Error_InvalidParams("Out of Gas Price Instrinsic").toJson(j_response);
		}
		else if (instanceof<dev::BlockGasLimitReached>(&e)) {
			RPC_Error_InvalidParams("Block Gas Limit Reached").toJson(j_response);
		}
		else if (instanceof<dev::GasPriceTooLow>(&e)) {
			RPC_Error_InvalidParams("Gas Price Too Low").toJson(j_response);
		}
		else if (instanceof<dev::ZeroSignatureTransaction>(&e)) {
			RPC_Error_InvalidParams("Zero Signature Transaction").toJson(j_response);
		}
		else if (instanceof<dev::InvalidNonce>(&e)) {
			RPC_Error_InvalidParams("Invalid Nounce").toJson(j_response);
		}
		else if (instanceof<dev::NotEnoughCash>(&e)) {
			RPC_Error_ServerProcedureSpecificationNotFound("Insufficient Balance").toJson(j_response);
		}
		else if (instanceof<dev::InvalidSignature>(&e)) {
			RPC_Error_InvalidParams("Invalid Signature").toJson(j_response);
		}
		else if (instanceof<dev::PendingTransactionAlreadyExists>(&e)) {//********
			RPC_Error_InternalError("Pending Transaction Already Exists").toJson(j_response);
		}
		else if (instanceof<dev::TransactionAlreadyInChain>(&e)) {//********
			RPC_Error_InternalError("Transaction already exists in chain").toJson(j_response);
		}
		else if (instanceof<dev::UnknownTransactionValidationError>(&e)) {//********
			RPC_Error_InternalError("Validation Error").toJson(j_response);
		}
		else if (instanceof<dev::AccountLocked>(&e)) {
			RPC_Error_InvalidParams("Locked Account").toJson(j_response);
		}
		else if (instanceof<dev::PendingTransactionTooMuch>(&e)) {//******
			RPC_Error_InternalError("Account's Pending transaction is full").toJson(j_response);
		}
		else if (instanceof<dev::UnknownAccount>(&e)) {
			RPC_Error_InvalidParams("Invalid Account").toJson(j_response);
		}
		else if (instanceof<dev::eth::ExtraDataTooBig>(&e)) {
			RPC_Error_InvalidParams("Data Too Large").toJson(j_response);
		}
		else if(instanceof<std::invalid_argument>(&e)){
			RPC_Error_InvalidParams(e.what()).toJson(j_response);
		}
		else if(instanceof<dev::BadHexCharacter>(&e)){
			RPC_Error_InvalidParams("Invalid Argument: Bad Hex Character").toJson(j_response);
		}
		else if(instanceof<mcp::json::exception>(&e)){
			RPC_Error_JsonParseError(e.what()).toJson(j_response);
		}
		else {
			RPC_Error_InvalidParams("Invalid Param").toJson(j_response);
		}
	}
}
