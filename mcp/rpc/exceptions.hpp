#pragma once

#include <libdevcore/Exceptions.h>

namespace mcp
{
	struct RpcException : virtual Exception
	{
		const char* what() const noexcept override { return "OK"; }
		const int virtual code() const noexcept { return 0; }
	};

#define RPC_ERROR_EXCEPTION(X, C, M)  \
    struct X : virtual RpcException \
    {                            \
		const char* what() const noexcept override { return M; } \
		const int code() const noexcept override { return C; } \
    }

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

	RPC_ERROR_EXCEPTION(RPC_Error_Eth_Ok, 0, "");
	RPC_ERROR_EXCEPTION(RPC_Error_Eth_PARSE_ERROR, -32700, "Parse error");
	RPC_ERROR_EXCEPTION(RPC_Error_Eth_INVALID_REQUEST, -32600, "Invalid request");
	RPC_ERROR_EXCEPTION(RPC_Error_Eth_METHOD_NOT_FOUND, -32601, "Method not found");
	RPC_ERROR_EXCEPTION(RPC_Error_Eth_INVALID_PARAMS, -32602, "Invalid params");
	RPC_ERROR_EXCEPTION(RPC_Error_Eth_INTERNAL_ERROR, -32603, "Internal error");
	RPC_ERROR_EXCEPTION(RPC_Error_Eth_METHOD_NOT_SUPPORTED, -32004, "Method not supported");
	RPC_ERROR_EXCEPTION(RPC_Error_Eth_INVALID_INPUT, -32000, "Invalid input");
	RPC_ERROR_EXCEPTION(RPC_Error_Eth_TRANSACTION_REJECTED, -32003, "Transaction rejected");

	RPC_ERROR_EXCEPTION(RPC_Error_UnknowError, 100, "Unkown Error");
}
