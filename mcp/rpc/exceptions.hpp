#pragma once

#include <libdevcore/Exceptions.h>

namespace mcp
{
	struct RpcException : virtual Exception
	{
		const char* what() const noexcept override { return "OK"; }
		const int virtual code() const noexcept { return 0; }
		const void virtual toJson(json & j_response) const noexcept {
			if (code() >= 0) {
				j_response["code"] = code();
			}
			j_response["msg"] = what();
		}
	};

	struct RpcEthException : RpcException
	{
		const void toJson(json & j_response) const noexcept override {
			json error;
			error["code"] = code();
			error["message"] = what();
			j_response["error"] = error;
		}
	};

#define RPC_ERROR_EXCEPTION(X, C, M)  \
    struct X : virtual RpcException \
    {                            \
		const char* what() const noexcept override { return M; } \
		const int code() const noexcept override { return C; } \
    }

#define RPC_ETH_ERROR_EXCEPTION(X, C, M)  \
    struct X : virtual RpcEthException \
    {                            \
		const char* what() const noexcept override { return M; } \
		const int code() const noexcept override { return C; } \
    }
	
	RPC_ERROR_EXCEPTION(RPC_Error_Disabled, -2, "RPC control is disabled");
	RPC_ERROR_EXCEPTION(RPC_Error_UnknownCommand, -1, "Unknown command");

	RPC_ERROR_EXCEPTION(RPC_Error_OK, 0, "OK");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidAccount, 1, "Invalid account");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidLengthPassword, 2, "Invalid password! A valid password length must between 8 and 100");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidCharactersPassword, 3, "Invalid password! A valid password must contain characters from letters (a-Z, A-Z), digits (0-9) and special characters (!@#$%^&*)");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidGenNextWorkValue, 4, "Invalid gen_next_work format");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidPassword, 5, "Invalid password");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidBackup, 6, "Invalid backup");
	RPC_ERROR_EXCEPTION(RPC_Error_AccountNotExist, 7, "Account not found");
	RPC_ERROR_EXCEPTION(RPC_Error_WrongPassword, 8, "Wrong password");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidJson, 9, "Invalid json");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidOldPassword, 10, "Invalid old password");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidNewPassword, 11, "Invalid new password");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidLimit, 12, "Invalid limit");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidLimitTooLarge, 13, "Limit too large, it can not be large than 1000"); // std::to_string(rpc_handler::list_max_limit));
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidIndex, 14, "Invalid index");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidIndexNotExsist, 15, "Index not found");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidAccountFrom, 16, "Invalid from account");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidAccountTo, 17, "Invalid to account");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidAmount, 18, "Invalid amount format");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidGas, 19, "Invalid gas format");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidData, 20, "Invalid data format");
	RPC_ERROR_EXCEPTION(RPC_Error_DataSizeTooLarge, 21, "Data size too large");
	RPC_ERROR_EXCEPTION(RPC_Error_AccountLocked, 22, "Account locked");
	RPC_ERROR_EXCEPTION(RPC_Error_InsufficientBalance, 23, "Insufficient balance");
	RPC_ERROR_EXCEPTION(RPC_Error_ValidateError, 24, "Validate error");
	RPC_ERROR_EXCEPTION(RPC_Error_SendBlockError, 25, "Send block error");
	RPC_ERROR_EXCEPTION(RPC_Error_SendUnknownError, 26, "Send block unkonw error");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidId, 27, "Invalid id");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidAsync, 28, "Invalid Async");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidGasPrice, 29, "Invalid gas price format");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidPrevious, 30, "Invalid Previous format");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidExecTimestamp, 31, "Invalid exec timestamp format");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidWork, 32, "Invalid work format");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidSignature, 33, "Invalid signature format");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidPublicKey, 34, "Invalid public key format");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidMsg, 35, "Invalid msg format");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidHash, 36, "Invalid hash format");
	RPC_ERROR_EXCEPTION(RPC_Error_BlockNotExsist, 37, "Hash not found");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidMci, 38, "Invalid mci");
	RPC_ERROR_EXCEPTION(RPC_Error_GasNotEnoughOrFail, 39, "Gas not enough or execute fail");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidBegin, 40, "Invalid begin address");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidMaxResults, 41, "Invalid value of max results");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidFromStableBlockIndex, 42, "Invalid from stable block index");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidToStableBlockIndex, 43, "Invalid to stable block index");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidTopics, 44, "Invalid topics");
	RPC_ERROR_EXCEPTION(RPC_Error_InvalidParams, 45, "Invalid params");
	RPC_ERROR_EXCEPTION(RPC_Error_EmptyPassword, 46, "Password can not be empty");
	RPC_ERROR_EXCEPTION(RPC_Error_VMException, 47, "Unexpected exception in VM. There may be a bug in this implementation.");
	RPC_ERROR_EXCEPTION(RPC_Error_PendingTransactionAlreadyExists, 48, "Pending transaction already exists");
	RPC_ERROR_EXCEPTION(RPC_Error_TransactionAlreadyInChain, 49, "Transaction already exists in chain");

	RPC_ERROR_EXCEPTION(RPC_Error_UnknowError, 100, "Unkown Error");

	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_OK, 0, "");
	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_InvalidAccount, -32602, "Invalid account");
	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_InvalidPassword, -32602, "Invalid password");
	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_LockedAccount, -32602, "Locked account");
	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_InvalidSignature, -32602, "Invalid signature");
	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_InvalidValue, -32602, "Invalid value");
	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_InvalidGas, -32602, "Invalid gas amount");
	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_InvalidGasPrice, -32602, "Invalid gas price");
	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_InvalidData, -32602, "Invalid data");
	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_InvalidNonce, -32602, "Invalid nonce");
	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_InvalidBlock, -32602, "Invalid block number");
	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_InvalidAccountFrom, -32602, "Invalid sender account");
	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_InvalidAccountTo, -32602, "Invalid receiver account");
	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_InvalidHash, -32602, "Invalid hash");
	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_InsufficientBalance, -32602, "Insufficient balance");
	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_DataTooLarge, -32602, "Data size is too large");
	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_Validation, -32603, "Validation error");
	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_PendingTransactionAlreadyExists, -32603, "Pending transaction already exists");
	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_TransactionAlreadyInChain, -32603, "Transaction already exists in chain");

	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_ParseError, -32700, "Parse error");
	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_InvalidRequest, -32600, "Invalid request");
	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_MethodNotFound, -32601, "Method not found");
	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_InvalidParams, -32602, "Invalid params");
	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_InternalError, -32603, "Internal error");
	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_MethodNotSupported, -32004, "Method not supported");
	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_InvalidInput, -32000, "Invalid input");
	RPC_ETH_ERROR_EXCEPTION(RPC_Error_Eth_TransactionRejected, -32003, "Transaction rejected");

	template<typename Base, typename T>
	inline bool instanceof(const T *ptr) {
		return dynamic_cast<const Base*>(ptr) != nullptr;
	}

	inline void toRpcExceptionJson(Exception const & e, json & j_response) {
		if (instanceof<dev::GasPriceTooLow>(&e) ||
			instanceof<dev::BlockGasLimitReached>(&e) ||
			instanceof<dev::GasPriceTooLow>(&e)) {
			RPC_Error_InvalidGas().toJson(j_response);
		}
		else if (instanceof<dev::NotEnoughCash>(&e)) {
			RPC_Error_InsufficientBalance().toJson(j_response);
		}
		else if (instanceof<dev::PendingTransactionAlreadyExists>(&e)) {
			RPC_Error_PendingTransactionAlreadyExists().toJson(j_response);
		}
		else if (instanceof<dev::TransactionAlreadyInChain>(&e)) {
			RPC_Error_TransactionAlreadyInChain().toJson(j_response);
		}
		else if (instanceof<dev::UnknownTransactionValidationError>(&e)) {
			RPC_Error_ValidateError().toJson(j_response);
		}
		else {
			RPC_Error_InvalidParams().toJson(j_response);
		}
	}

	inline void toRpcExceptionEthJson(Exception const & e, json & j_response) {
		if (instanceof<dev::OutOfGasIntrinsic>(&e) ||
			instanceof<dev::BlockGasLimitReached>(&e) ||
			instanceof<dev::GasPriceTooLow>(&e)) {
			RPC_Error_Eth_InvalidGas().toJson(j_response);
		}
		else if (instanceof<dev::InvalidNonce>(&e)) {
			RPC_Error_Eth_InvalidNonce().toJson(j_response);
		}
		else if (instanceof<dev::NotEnoughCash>(&e)) {
			RPC_Error_Eth_InsufficientBalance().toJson(j_response);
		}
		else if (instanceof<dev::PendingTransactionAlreadyExists>(&e)) {
			RPC_Error_Eth_PendingTransactionAlreadyExists().toJson(j_response);
		}
		else if (instanceof<dev::TransactionAlreadyInChain>(&e)) {
			RPC_Error_Eth_TransactionAlreadyInChain().toJson(j_response);
		}
		else if (instanceof<dev::UnknownTransactionValidationError>(&e)) {
			RPC_Error_Eth_Validation().toJson(j_response);
		}
		else {
			RPC_Error_Eth_InvalidParams().toJson(j_response);
		}
	}
}
