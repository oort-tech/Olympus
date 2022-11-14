/*

    Copyright 2019 dYdX Trading Inc.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

*/

pragma solidity 0.5.7;
pragma experimental ABIEncoderV2;

import { SafeMath } from "openzeppelin-solidity/contracts/math/SafeMath.sol";
import { Ownable } from "openzeppelin-solidity/contracts/ownership/Ownable.sol";
import { IAutoTrader } from "../../protocol/interfaces/IAutoTrader.sol";
import { ICallee } from "../../protocol/interfaces/ICallee.sol";
import { Account } from "../../protocol/lib/Account.sol";
import { Math } from "../../protocol/lib/Math.sol";
import { Require } from "../../protocol/lib/Require.sol";
import { Types } from "../../protocol/lib/Types.sol";
import { OnlySolo } from "../helpers/OnlySolo.sol";
import { TypedSignature } from "../lib/TypedSignature.sol";


/**
 * @title LimitOrders
 * @author dYdX
 *
 * Allows for Limit Orders to be used with dYdX
 */
contract LimitOrders is
    Ownable,
    OnlySolo,
    IAutoTrader,
    ICallee
{
function coverage_0x729d531c(bytes32 c__0x729d531c) public pure {}

    using Math for uint256;
    using SafeMath for uint256;
    using Types for Types.Par;
    using Types for Types.Wei;

    // ============ Constants ============

    bytes32 constant private FILE = "LimitOrders";

    // EIP191 header for EIP712 prefix
    bytes2 constant private EIP191_HEADER = 0x1901;

    // EIP712 Domain Name value
    string constant private EIP712_DOMAIN_NAME = "LimitOrders";

    // EIP712 Domain Version value
    string constant private EIP712_DOMAIN_VERSION = "1.1";

    // Hash of the EIP712 Domain Separator Schema
    /* solium-disable-next-line indentation */
    bytes32 constant private EIP712_DOMAIN_SEPARATOR_SCHEMA_HASH = keccak256(abi.encodePacked(
        "EIP712Domain(",
        "string name,",
        "string version,",
        "uint256 chainId,",
        "address verifyingContract",
        ")"
    ));

    // Hash of the EIP712 LimitOrder struct
    /* solium-disable-next-line indentation */
    bytes32 constant private EIP712_LIMIT_ORDER_STRUCT_SCHEMA_HASH = keccak256(abi.encodePacked(
        "LimitOrder(",
        "uint256 makerMarket,",
        "uint256 takerMarket,",
        "uint256 makerAmount,",
        "uint256 takerAmount,",
        "address makerAccountOwner,",
        "uint256 makerAccountNumber,",
        "address takerAccountOwner,",
        "uint256 takerAccountNumber,",
        "uint256 expiration,",
        "uint256 salt",
        ")"
    ));

    // Number of bytes in an Order struct
    uint256 constant private NUM_ORDER_BYTES = 320;

    // Number of bytes in a typed signature
    uint256 constant private NUM_SIGNATURE_BYTES = 66;

    // Number of bytes in a CallFunctionData struct
    uint256 constant private NUM_CALLFUNCTIONDATA_BYTES = 32 + NUM_ORDER_BYTES;

    // ============ Enums ============

    enum OrderStatus {
        Null,
        Approved,
        Canceled
    }

    enum CallFunctionType {
        Approve,
        Cancel
    }

    // ============ Structs ============

    struct Order {
        uint256 makerMarket;
        uint256 takerMarket;
        uint256 makerAmount;
        uint256 takerAmount;
        address makerAccountOwner;
        uint256 makerAccountNumber;
        address takerAccountOwner;
        uint256 takerAccountNumber;
        uint256 expiration;
        uint256 salt;
    }

    struct OrderInfo {
        Order order;
        bytes32 orderHash;
    }

    struct CallFunctionData {
        CallFunctionType callType;
        Order order;
    }

    struct OrderQueryOutput {
        OrderStatus orderStatus;
        uint256 orderMakerFilledAmount;
    }

    // ============ Events ============

    event ContractStatusSet(
        bool operational
    );

    event LogLimitOrderCanceled(
        bytes32 indexed orderHash,
        address indexed canceler,
        uint256 makerMarket,
        uint256 takerMarket
    );

    event LogLimitOrderApproved(
        bytes32 indexed orderHash,
        address indexed approver,
        uint256 makerMarket,
        uint256 takerMarket
    );

    event LogLimitOrderFilled(
        bytes32 indexed orderHash,
        address indexed orderMaker,
        uint256 makerFillAmount,
        uint256 totalMakerFilledAmount
    );

    // ============ Immutable Storage ============

    // Hash of the EIP712 Domain Separator data
    bytes32 public EIP712_DOMAIN_HASH;

    // ============ Mutable Storage ============

    // true if this contract can process orders
    bool public g_isOperational;

    // order hash => filled amount (in makerAmount)
    mapping (bytes32 => uint256) public g_makerFilledAmount;

    // order hash => status
    mapping (bytes32 => OrderStatus) public g_status;

    // ============ Constructor ============

    constructor (
        address soloMargin,
        uint256 chainId
    )
        public
        OnlySolo(soloMargin)
    {coverage_0x729d531c(0x77a52a5429190e98171948ccf17f6f0362b8fc5e206fadca50f7a4badb4fb6e8); /* function */ 

coverage_0x729d531c(0x80edba06f5a38dc45beb946e42a0b212c586a02614e85f10387460d0c035bc4d); /* line */ 
        coverage_0x729d531c(0x0dc7d96a06625b33e0a3ff03ea332c311c759b0546961c77cef001564c653957); /* statement */ 
g_isOperational = true;

        /* solium-disable-next-line indentation */
coverage_0x729d531c(0xc2d2cdab4c7f1fb1cfe396c04523d6adb8e8aa3ef93cfa5144d5b3db542b480d); /* line */ 
        coverage_0x729d531c(0x831a2107918e43fd35a80a6a9626e346fa08719ed6d5f6308922104979979135); /* statement */ 
EIP712_DOMAIN_HASH = keccak256(abi.encode(
            EIP712_DOMAIN_SEPARATOR_SCHEMA_HASH,
            keccak256(bytes(EIP712_DOMAIN_NAME)),
            keccak256(bytes(EIP712_DOMAIN_VERSION)),
            chainId,
            address(this)
        ));
    }

    // ============ Admin Functions ============

    /**
     * The owner can shut down the exchange.
     */
    function shutDown()
        external
        onlyOwner
    {coverage_0x729d531c(0xecbe5bc0d2bdf1dc0c24e0f7b1714327bc6f9e29d06e1c968886741cbc401fb4); /* function */ 

coverage_0x729d531c(0x6ed4e82a7a0f630381d212960e350a6d61663b9c1fd6ac2b7176cbc9e6f861b7); /* line */ 
        coverage_0x729d531c(0x4f6a2e2f1b9c092cafbd1b35c4f9a9b5df5978ac2e4f293c4934ee76f985a776); /* statement */ 
g_isOperational = false;
coverage_0x729d531c(0x9f79c8ed052d74f7e6598c663dda68a68d4911d323c383923b5e863803dcdb0e); /* line */ 
        coverage_0x729d531c(0xb91098625c95b2826feeb3d996d66522afab694de377c08f1cb9ee79b4f7e804); /* statement */ 
emit ContractStatusSet(false);
    }

    /**
     * The owner can start back up the exchange.
     */
    function startUp()
        external
        onlyOwner
    {coverage_0x729d531c(0xcd6ad6605e51f887375978b2f029b556985f5a8fa6c471cbd0501f2d697faaef); /* function */ 

coverage_0x729d531c(0xefd50cf8f096ddd1be910a4209e8d4369a83b4612de959339d08a3cae01c014c); /* line */ 
        coverage_0x729d531c(0x1c10af248c491e974d242fb959238e577d08aebf471d6c3a297c824838219f36); /* statement */ 
g_isOperational = true;
coverage_0x729d531c(0xd3c06d2f2e646ae7388f72361c4e8f95e80701da6ed282425c804d26e3f992f8); /* line */ 
        coverage_0x729d531c(0xf1511f0024a97d83cd4fcc04517da83b81c80ad6b2987abcf9a2e7c5f8985bd4); /* statement */ 
emit ContractStatusSet(true);
    }

    // ============ External Functions ============

    /**
     * Cancels an order. Cannot already be canceled.
     *
     * @param  order  The order to cancel
     */
    function cancelOrder(
        Order memory order
    )
        public
    {coverage_0x729d531c(0x94a1d585138bc75b9a2be44bb2847359cdf358c97dba581cbd8ec68ce1a55fcb); /* function */ 

coverage_0x729d531c(0x811a58a384813e0515e5bf19ef24bece6e312a1057259c1d18fd67ae0c11fbd1); /* line */ 
        coverage_0x729d531c(0xcbb82fc68abc475a62d3c35aa06261a3f1b6c7013363b4363342c5c35afcb86f); /* statement */ 
cancelOrderInternal(msg.sender, order);
    }

    /**
     * Approves an order. Cannot already be approved or canceled.
     *
     * @param  order  The order to approve
     */
    function approveOrder(
        Order memory order
    )
        public
    {coverage_0x729d531c(0x5d0bdfbe3fe17846a4759dbbc9bc6fa2df1dfe4553e9185894ddacb561c0738c); /* function */ 

coverage_0x729d531c(0xe5237e1bde62b34ecbb42bb66886385b796a90ae3456ac3b2e9795bd36454f87); /* line */ 
        coverage_0x729d531c(0x61107c00c4fa8afac80aa11db8f6923eb9a763208d2e8d8a5c1ded3f5bbf658a); /* statement */ 
approveOrderInternal(msg.sender, order);
    }

    // ============ Only-Solo Functions ============

    /**
     * Allows traders to make trades approved by this smart contract. The active trader's account is
     * the takerAccount and the passive account (for which this contract approves trades
     * on-behalf-of) is the makerAccount.
     *
     * @param  inputMarketId   The market for which the trader specified the original amount
     * @param  outputMarketId  The market for which the trader wants the resulting amount specified
     * @param  makerAccount    The account for which this contract is making trades
     * @param  takerAccount    The account requesting the trade
     *  param  oldInputPar     (unused)
     *  param  newInputPar     (unused)
     * @param  inputWei        The change in token amount for the makerAccount for the inputMarketId
     * @param  data            Arbitrary data passed in by the trader
     * @return                 The AssetAmount for the makerAccount for the outputMarketId
     */
    function getTradeCost(
        uint256 inputMarketId,
        uint256 outputMarketId,
        Account.Info memory makerAccount,
        Account.Info memory takerAccount,
        Types.Par memory /* oldInputPar */,
        Types.Par memory /* newInputPar */,
        Types.Wei memory inputWei,
        bytes memory data
    )
        public
        onlySolo(msg.sender)
        returns (Types.AssetAmount memory)
    {coverage_0x729d531c(0x158bcc7e45f866ce45233e23585b516da5d298dd7fe914b6dffc056faa4060b9); /* function */ 

coverage_0x729d531c(0x11eff8b44adc756b9dea8ee7de77a57ec0a4e6b6aac33d44156d4d2cec52a9bc); /* line */ 
        coverage_0x729d531c(0x338822c850b9555fae9ac7b4b76697609aa0c1c543b96f82274f72b111f3be20); /* statement */ 
Require.that(
            g_isOperational,
            FILE,
            "Contract is not operational"
        );

coverage_0x729d531c(0x29764dee1bb337778ee33d6557d1d6f7b65b897460c11a66658163c4f8d1b1b0); /* line */ 
        coverage_0x729d531c(0x7efa4cc395c55f2031fa8fcf94d8f147a96d16e153a8a0f32844c482a6445dd0); /* statement */ 
OrderInfo memory orderInfo = getOrderAndValidateSignature(data);

coverage_0x729d531c(0x493992a6c1dfdbe846cf900eba9db0187165bfdb1246ed8b4b67dc1511fd5038); /* line */ 
        coverage_0x729d531c(0x39adcb3cc71b980b1e58ee91431aca61b3e4442f4e80a88e8cc4c194b9a3f5a5); /* statement */ 
verifyOrderAndAccountsAndMarkets(
            orderInfo,
            makerAccount,
            takerAccount,
            inputMarketId,
            outputMarketId,
            inputWei
        );

coverage_0x729d531c(0xb3fa7eaa3e8b05ede2123d324bf027450f2378417bd33dbbb1e8809d1e98bbf1); /* line */ 
        coverage_0x729d531c(0x835eb07894c379578dcdce3c4cefe0fc01421d4d4bc42be2a77a0d2dc7a6391a); /* statement */ 
return getOutputAssetAmount(
            inputMarketId,
            outputMarketId,
            inputWei,
            orderInfo
        );
    }

    /**
     * Allows users to send this contract arbitrary data.
     *
     *  param  sender       (unused)
     * @param  accountInfo  The account from which the data is being sent
     * @param  data         Arbitrary data given by the sender
     */
    function callFunction(
        address /* sender */,
        Account.Info memory accountInfo,
        bytes memory data
    )
        public
        onlySolo(msg.sender)
    {coverage_0x729d531c(0x36da14d009b9033c0a52ff0f40dd06c98ccf22b9b1709ecc3c74863d47456b35); /* function */ 

coverage_0x729d531c(0x6c14870b546c7afa519ad14337d908033b807938df86d288d29b9e7b5a098925); /* line */ 
        coverage_0x729d531c(0x53dc1f09d3c42fd53fa2ba50c7f858b623af83e254c13f7b2527d1ebde72d6b0); /* statement */ 
Require.that(
            data.length == NUM_CALLFUNCTIONDATA_BYTES,
            FILE,
            "Cannot parse CallFunctionData"
        );

coverage_0x729d531c(0xdd9fff1b0212a32a571a0084196146c562631f7e89abec3bb33ee17e5fcd30b8); /* line */ 
        coverage_0x729d531c(0x76661cdbb4e9337e7fb3c0f2ef1cf1478a328067ffa8fd03085f321af94480d1); /* statement */ 
CallFunctionData memory cfd = abi.decode(data, (CallFunctionData));

coverage_0x729d531c(0x5cc18474ae36a6d5a6f4289f4473abf7867f66e2fef2ae302f7a6dc15b6bac28); /* line */ 
        coverage_0x729d531c(0xa50def55bde146cccfbf08e5b12f9a5f29b6749d9182afcbcd1f7a95942de206); /* statement */ 
if (cfd.callType == CallFunctionType.Approve) {coverage_0x729d531c(0xb795951a9a14bc51788b0d472aa016ed475b8c8280d2a3c49c3e32cc9699e855); /* branch */ 

coverage_0x729d531c(0x72d0ac844d397670c243b40fa4807390bf23ec2c03794a7dce4c9428974a64e4); /* line */ 
            coverage_0x729d531c(0x81628e965bdd37382f13e13190ed4cbb11d621852db92eb48673bc9319ec8f79); /* statement */ 
approveOrderInternal(accountInfo.owner, cfd.order);
        } else {coverage_0x729d531c(0x7469c9bdc65d2be1eca2d7f51b2467ff33b8d386eb17cf7c3f43662b5b4199e0); /* branch */ 

coverage_0x729d531c(0x65a82201913ceadd1f3c77cf3a85a0498e184ce6f6545f50c71ce1de7ddcf001); /* line */ 
            coverage_0x729d531c(0x3cb807805df93f2dbe60802a541ef053616e175a0292f20aa0606a93b9794e6c); /* assertPre */ 
coverage_0x729d531c(0x1bf49ed397ceb30ede3b54ec93c9fe0d8e5c09eb4dfaa6b52390aa5f3e2ec2c9); /* statement */ 
assert(cfd.callType == CallFunctionType.Cancel);coverage_0x729d531c(0xcfa0346fc122501e66e6b4421401b9edee6fef5633de1258290a5338ce836d4a); /* assertPost */ 

coverage_0x729d531c(0x9f023a0247ad4aaa575b6670957181bd4e8cf66926271fb446c295cda3230b79); /* line */ 
            coverage_0x729d531c(0x3117b12e8162f9b7a827a2d06f1db6add210e0fe7b74bb7df7a60455ef902bfb); /* statement */ 
cancelOrderInternal(accountInfo.owner, cfd.order);
        }
    }

    // ============ Getters ============

    /**
     * Returns the status and the filled amount (in makerAmount) of several orders.
     */
    function getOrderStates(
        bytes32[] memory orderHashes
    )
        public
        view
        returns(OrderQueryOutput[] memory)
    {coverage_0x729d531c(0x401f95178eab722e1d5ff82f47a82b552956d6c05abad9b3b2ce76211415e8b5); /* function */ 

coverage_0x729d531c(0x92fc7f848cbb15b3b7e431f7935dcfcb1781af61c659c45b46a9076180ec1a2a); /* line */ 
        coverage_0x729d531c(0xfc8bce31cf86c2b4b3d93a5a2de376ea98b8be3e551d3f39c33b927a7304ce80); /* statement */ 
uint256 numOrders = orderHashes.length;
coverage_0x729d531c(0x5799ff6645a91a305c18578efbd500cc5f6b9184e41a31c945a94e286a468a52); /* line */ 
        coverage_0x729d531c(0xaf3ab0acc007b3d184527866c80e5bcf7e552b9f7a37d54c8836aeb7302d583c); /* statement */ 
OrderQueryOutput[] memory output = new OrderQueryOutput[](numOrders);

        // for each order
coverage_0x729d531c(0xb457f21051e0c72ac7efe79935323d4b6a0b361eb988549794e0b7d8cba13aa5); /* line */ 
        coverage_0x729d531c(0xbeae5642d506e62d27a0e217fcab9e765c600c72aa8800e4daeb49d53f9b962a); /* statement */ 
for (uint256 i = 0; i < numOrders; i++) {
coverage_0x729d531c(0xc57d59d0e0261afcbe7b83d3212952fd9e585e29a91b1f54faca8bb189317378); /* line */ 
            coverage_0x729d531c(0x3df54db9989cae2288224d411e2397523bf11788fd9ed43fe16c5a43e19a9e77); /* statement */ 
bytes32 orderHash = orderHashes[i];
coverage_0x729d531c(0x0d19f31ddde597159c42a4d5100801b719816d89f08a82deaba3bddf8c240c04); /* line */ 
            coverage_0x729d531c(0x6f5e988f47cded8f7905da7ee6b94ac43500e33a66c4d3a31563e870ef6e3d83); /* statement */ 
output[i] = OrderQueryOutput({
                orderStatus: g_status[orderHash],
                orderMakerFilledAmount: g_makerFilledAmount[orderHash]
            });
        }
coverage_0x729d531c(0xecd83172855c24623435093d74c153e4eda9c1d28e253203bed329f8f89fa5bf); /* line */ 
        coverage_0x729d531c(0x5e76ff1e8d69ac1b7a023ac19ce8475e2cae10eba78a1153274f0614cc779d95); /* statement */ 
return output;
    }

    // ============ Private Storage Functions ============

    /**
     * Cancels an order as long as it is not already canceled.
     */
    function cancelOrderInternal(
        address canceler,
        Order memory order
    )
        private
    {coverage_0x729d531c(0xfcdeae010b1037f8673d75be692527ab4e7d9dc89be66511f9265037ea8acaa8); /* function */ 

coverage_0x729d531c(0x382de9aa508d6b6af24205a87ce7679bf6c71cc304bc5646f76c0836bb4b37eb); /* line */ 
        coverage_0x729d531c(0xd26d123435cf3de418ec9cfcaae01a4a4e416193514d0b109ce2bec2a9e8f4b2); /* statement */ 
Require.that(
            canceler == order.makerAccountOwner,
            FILE,
            "Canceler must be maker"
        );
coverage_0x729d531c(0xc74be22ca7b49b7dd99832b3eba218aea4247b1bbf096af05bde7d1e3158662e); /* line */ 
        coverage_0x729d531c(0x31354573ac4adb1655fda71824b55c692bf86ed5adb0b1a60db16d660d8cca31); /* statement */ 
bytes32 orderHash = getOrderHash(order);
coverage_0x729d531c(0x17e891fad046dfe4bae3bbe4991a009e93c42790a98ab05c8982d3a06aa7256a); /* line */ 
        coverage_0x729d531c(0x62d6e80ca074688e4ec67b109ae3fd4109e831decf863dc0135b4eeb09352bb9); /* statement */ 
g_status[orderHash] = OrderStatus.Canceled;
coverage_0x729d531c(0x15a8ea71d257fa40fcefeeb00ae69c52dd202c0096c613ec1bcbcb97a57c35d1); /* line */ 
        coverage_0x729d531c(0x63e4a78c2f62558555a86c1528fa57decf884cd7734ca45b69999cb4b3f95c74); /* statement */ 
emit LogLimitOrderCanceled(
            orderHash,
            canceler,
            order.makerMarket,
            order.takerMarket
        );
    }

    /**
     * Approves an order as long as it is not already approved or canceled.
     */
    function approveOrderInternal(
        address approver,
        Order memory order
    )
        private
    {coverage_0x729d531c(0x179c9b982f43c1ae41362e30860a0a98e110fcc66dc288639ab1fa44414ea5a4); /* function */ 

coverage_0x729d531c(0x88cbb077409b478196a344e0013347a72fd150b2794a77d602307f25a79e8057); /* line */ 
        coverage_0x729d531c(0x91621b28aaf4cf7d07e9f5f39878805cc76d4f32867f8fd65a31e5d17f53debf); /* statement */ 
Require.that(
            approver == order.makerAccountOwner,
            FILE,
            "Approver must be maker"
        );
coverage_0x729d531c(0x8d107882027ec6f839dbf9c5a64925b81c8bfc22320f2bb104d041dbcf31d36e); /* line */ 
        coverage_0x729d531c(0xb4c6c438c42a8dff27b28a4d26c35b1c464b3602cb87ce1cc9072d89c6c07116); /* statement */ 
bytes32 orderHash = getOrderHash(order);
coverage_0x729d531c(0x86e53d30b9fff81dbc60929e6254d7c36c2f82544281bd8095b0370d86f97edb); /* line */ 
        coverage_0x729d531c(0x52c1f2640ec7edf438e846c2a0e68fd9ad3de19430766466011fe9421f316a7f); /* statement */ 
Require.that(
            g_status[orderHash] != OrderStatus.Canceled,
            FILE,
            "Cannot approve canceled order",
            orderHash
        );
coverage_0x729d531c(0x2f6f7d768df383da50d3748296197959d3d855983455549120e11450d56b3b77); /* line */ 
        coverage_0x729d531c(0x4ec58926848f680e678609f55104272ead02b67c457412fa8445c9ca789fa59b); /* statement */ 
g_status[orderHash] = OrderStatus.Approved;
coverage_0x729d531c(0xd39f070966a95023996322bd95ffd9368ad44990be2d820c4db43109e4ccd315); /* line */ 
        coverage_0x729d531c(0x5807fe8b289868f7bf07a6e38cc1f1eeda187b25b89800eb2b3aa27d27f49d7a); /* statement */ 
emit LogLimitOrderApproved(
            orderHash,
            approver,
            order.makerMarket,
            order.takerMarket
        );
    }

    // ============ Private Helper Functions ============

    /**
     * Verifies that the order is still fillable for the particular accounts and markets specified.
     */
    function verifyOrderAndAccountsAndMarkets(
        OrderInfo memory orderInfo,
        Account.Info memory makerAccount,
        Account.Info memory takerAccount,
        uint256 inputMarketId,
        uint256 outputMarketId,
        Types.Wei memory inputWei
    )
        private
        view
    {coverage_0x729d531c(0x3d02764828373169eb0ff751b8d27d1e51885f92497eb22dcf12f4d9dc51330d); /* function */ 

        // verify expriy
coverage_0x729d531c(0x852798e88d40fe95ff5f509a7ff9da275f3efdf585e6016d172f3564626bacd2); /* line */ 
        coverage_0x729d531c(0x650110d973140f887c0e22f7c8dbda7187a9c26f2cf5eef08eff49c6ea892105); /* statement */ 
Require.that(
            orderInfo.order.expiration == 0 || orderInfo.order.expiration >= block.timestamp,
            FILE,
            "Order expired",
            orderInfo.orderHash
        );

        // verify maker
coverage_0x729d531c(0x0bb650bed1038c5e6581de7d2a19b6fa8e44f76af9868ba5ff67183866efabf1); /* line */ 
        coverage_0x729d531c(0xab06a46d1252090c3faa019e2272f0c3ac295a9c7ccdacac382af5e14dff1322); /* statement */ 
Require.that(
            makerAccount.owner == orderInfo.order.makerAccountOwner &&
            makerAccount.number == orderInfo.order.makerAccountNumber,
            FILE,
            "Order maker account mismatch",
            orderInfo.orderHash
        );

        // verify taker
coverage_0x729d531c(0xf0bcc4c82683a4eb9545d19a80e9fae02f183c4666263fda7a4128fb025e893d); /* line */ 
        coverage_0x729d531c(0x535c684ff25229f2fd6c221476acdac759889c4039229ea4650a7cf347a521f5); /* statement */ 
Require.that(
            (
                orderInfo.order.takerAccountOwner == address(0) &&
                orderInfo.order.takerAccountNumber == 0
            ) || (
                orderInfo.order.takerAccountOwner == takerAccount.owner &&
                orderInfo.order.takerAccountNumber == takerAccount.number
            ),
            FILE,
            "Order taker account mismatch",
            orderInfo.orderHash
        );

        // verify markets
coverage_0x729d531c(0x84fafb01e32426602d239bd62f966c7cbf3a51cc43c0b4ae1d5448ac8d429ba1); /* line */ 
        coverage_0x729d531c(0x31729d6875dca6400c4b2d59d82770bb21b6ecf20c696a0d13a3e2a73be0fded); /* statement */ 
Require.that(
            (
                orderInfo.order.makerMarket == outputMarketId &&
                orderInfo.order.takerMarket == inputMarketId
            ) || (
                orderInfo.order.takerMarket == outputMarketId &&
                orderInfo.order.makerMarket == inputMarketId
            ),
            FILE,
            "Market mismatch",
            orderInfo.orderHash
        );

        // verify inputWei
coverage_0x729d531c(0x4ba21e47ce98d1186c4a38fccea7d78262a1ed0351b20e270e677fe909672668); /* line */ 
        coverage_0x729d531c(0xef0870f9d4a59498c01468c86c853b862d4ef427c5afbbae0be2e4bb1d7e8294); /* statement */ 
Require.that(
            !inputWei.isZero(),
            FILE,
            "InputWei is zero",
            orderInfo.orderHash
        );
coverage_0x729d531c(0x5237a147b80b1394715c022431a488086510d264f1b6ed51ed8c81de9f03237b); /* line */ 
        coverage_0x729d531c(0x133dbc7d243b812ab6ed25c7f91f10c93eeebb31268d899b18ca9c063ce32cd4); /* statement */ 
Require.that(
            inputWei.sign == (orderInfo.order.takerMarket == inputMarketId),
            FILE,
            "InputWei sign mismatch",
            orderInfo.orderHash
        );
    }

    /**
     * Returns the AssetAmount for the outputMarketId given the order and the inputs. Updates the
     * filled amount of the order in storage.
     */
    function getOutputAssetAmount(
        uint256 inputMarketId,
        uint256 outputMarketId,
        Types.Wei memory inputWei,
        OrderInfo memory orderInfo
    )
        private
        returns (Types.AssetAmount memory)
    {coverage_0x729d531c(0x38f58d8c7aa2eede2d849640c3b82e7252a90c2b67365f0cd5a4cf92eaf724ce); /* function */ 

coverage_0x729d531c(0x7d9a81461de289adbe13b12a899d5e37157548717b196577bd1031113f723309); /* line */ 
        coverage_0x729d531c(0x9444bac5aa880005dbceb5b0c1f68027ad708af29e45967705a8f65ba57d6989); /* statement */ 
uint256 outputAmount;
coverage_0x729d531c(0xfc5ccb62caf09165dd6f3b5a9e6332ab557ba6aa1e17f731c407a1a7e3c33d01); /* line */ 
        coverage_0x729d531c(0xbc1cc006d20a1be552d2f6acbbe27bfef56d8f1e02cf1a259b79c423066674e0); /* statement */ 
uint256 makerFillAmount;

coverage_0x729d531c(0x99866342b2ebe09a85edcad5e0e84b2f883f61f8cbd5a2185030c96aa9dfe593); /* line */ 
        coverage_0x729d531c(0x66cdca556aacad570766288d0960ccaccf755d6bfdee19c7f0e6545ffc846ac5); /* statement */ 
if (orderInfo.order.takerMarket == inputMarketId) {coverage_0x729d531c(0x1ffdae530de10ec3365af3f9dca4537c99f3ad87d95252df32765fb517e92303); /* branch */ 

coverage_0x729d531c(0xfc28966c176f8d5656272e59b16528f2fe350020d68d949510eca8c5623a3250); /* line */ 
            coverage_0x729d531c(0xd62aad812dcace305ccdf81995eeef79f87f14843a18e48c1591ec9891346a17); /* statement */ 
outputAmount = inputWei.value.getPartial(
                orderInfo.order.makerAmount,
                orderInfo.order.takerAmount
            );
coverage_0x729d531c(0xd11d66d08553bb99772454a5cbf1d78a9d4ac134a39f2ac81646515ec170099a); /* line */ 
            coverage_0x729d531c(0x80fa58392efc8657c911aa69edd1f014b1c9950827bb7e6c3fdcd53f2d95be0a); /* statement */ 
makerFillAmount = outputAmount;
        } else {coverage_0x729d531c(0x82e2421345e183f0911ba35aa3aa00675a4578c610b2cefa6c38b7edb0dd16ab); /* branch */ 

coverage_0x729d531c(0xe4b45df79ed3814be64a1b30750a51434c04feb2ecd1335003b4304be078bb64); /* line */ 
            coverage_0x729d531c(0x6330146b276ed10bcd29a1eaf2e49166b570cafea92d76643809e52a48283a68); /* assertPre */ 
coverage_0x729d531c(0x0a83370f81b46d82d728c49c70fede573524fd72bbad7ad74099fc52bcef9220); /* statement */ 
assert(orderInfo.order.takerMarket == outputMarketId);coverage_0x729d531c(0x7568753171568fb8d6967df468fd523dcfd4e16e510a6a6a3bfaa970821ba862); /* assertPost */ 

coverage_0x729d531c(0x2d399d0aeab2a6707aa3b04ff5305ff3a96f789b7d7568ad37a65b54c842e39b); /* line */ 
            coverage_0x729d531c(0x04281bb82ca900137d1f783d45ec762bd150ef533a78eb37c97ba29fff39aa78); /* statement */ 
outputAmount = inputWei.value.getPartialRoundUp(
                orderInfo.order.takerAmount,
                orderInfo.order.makerAmount
            );
coverage_0x729d531c(0x02ea7ba81bc3361ad50cfff8bfac232cae2f60d4d8b946357c7ff2a4b131403c); /* line */ 
            coverage_0x729d531c(0x074e4d9279a2a84ae5065a13fa48fc35c5ed86bbdbd17509c8a331fdf4eccbbc); /* statement */ 
makerFillAmount = inputWei.value;
        }

coverage_0x729d531c(0xc9f4337042e60e78e5a10b08e01c79150377895e69dc021d2a279b736d4dab06); /* line */ 
        coverage_0x729d531c(0x2127ae25a3ee7a893d706528b0d457579e0762eb22f164e89051baa5a6862af2); /* statement */ 
updateMakerFilledAmount(orderInfo, makerFillAmount);

coverage_0x729d531c(0x9992c5cc833ff7f7a57be18b70c732eda1b33423d4d4c9e4bfaaf041c7bedde1); /* line */ 
        coverage_0x729d531c(0x60ccbf159ee068f52c5a277d03e7258c1dd7691b52d2625bbde9bd459ed6d35a); /* statement */ 
return Types.AssetAmount({
            sign: orderInfo.order.takerMarket == outputMarketId,
            denomination: Types.AssetDenomination.Wei,
            ref: Types.AssetReference.Delta,
            value: outputAmount
        });
    }

    /**
     * Increases the stored filled amount (in makerAmount) of the order by makerFillAmount.
     * Returns the new total filled amount (in makerAmount).
     */
    function updateMakerFilledAmount(
        OrderInfo memory orderInfo,
        uint256 makerFillAmount
    )
        private
    {coverage_0x729d531c(0xec31d3e60ea3ae7b46b617bb82bf9c57474d7cf17d3b57a9acfca0ceb24e167f); /* function */ 

coverage_0x729d531c(0x874df67509c4ae64ff2283f2a054dd47902d63323b11cfc2fc8d6db3e8d53319); /* line */ 
        coverage_0x729d531c(0xf2501730384abfac541512e5b1888aa0625377c44da42bdc753b4d0f32466ed8); /* statement */ 
uint256 oldMakerFilledAmount = g_makerFilledAmount[orderInfo.orderHash];
coverage_0x729d531c(0xe02cd4a0497077f9ef9dfc957929a0c63f56c3bd33a8ffdd7bffa60fa50b6fd5); /* line */ 
        coverage_0x729d531c(0x73cd01b23a1523e1074256b867a57e31e4cdd02db2046d5904d8440a9ff5df28); /* statement */ 
uint256 totalMakerFilledAmount = oldMakerFilledAmount.add(makerFillAmount);
coverage_0x729d531c(0x07a15b65c969aab20dbbba27722f97b0e78b0c536620b6f0fc7384fc14979389); /* line */ 
        coverage_0x729d531c(0x9cd7c0579ff0046f9c4fabe64b3e499d91afb549f7342b29a84057911059f0d2); /* statement */ 
Require.that(
            totalMakerFilledAmount <= orderInfo.order.makerAmount,
            FILE,
            "Cannot overfill order",
            orderInfo.orderHash,
            oldMakerFilledAmount,
            makerFillAmount
        );

coverage_0x729d531c(0xe164544ec763c279c826c00dd309bd1f0884c5e0b3049bd5c1c51c11b29e811d); /* line */ 
        coverage_0x729d531c(0x4aeb2252ddac3b254865ee862b442586220fb30318071a096c9c6b7b2e35b5dd); /* statement */ 
g_makerFilledAmount[orderInfo.orderHash] = totalMakerFilledAmount;

coverage_0x729d531c(0x9b9ff0b0e2bade564fa639da204703ebd8fa74d8cba7fc160f05b07f630942fd); /* line */ 
        coverage_0x729d531c(0xb398798ead0ab3f0ad12de40dab836f7783544d3fff34a5425290690e9c0de2f); /* statement */ 
emit LogLimitOrderFilled(
            orderInfo.orderHash,
            orderInfo.order.makerAccountOwner,
            makerFillAmount,
            totalMakerFilledAmount
        );
    }

    /**
     * Parses the order, verifies that it is not expired or canceled, and verifies the signature.
     */
    function getOrderAndValidateSignature(
        bytes memory data
    )
        private
        view
        returns (OrderInfo memory)
    {coverage_0x729d531c(0xd1d19e0fec7990167097b687714d8fd9d2709c560a672f6656ddd4ec32ff9586); /* function */ 

coverage_0x729d531c(0x54731ed05128c13a0ab893fa8d67a69f7fd1d3fec208237cf684b5b51d5c2424); /* line */ 
        coverage_0x729d531c(0x74574d2bd57ecd34fcb7c51fe2a4880f45ca4070c6a54b749f0a12485d9e2d44); /* statement */ 
Require.that(
            (
                data.length == NUM_ORDER_BYTES ||
                data.length == NUM_ORDER_BYTES + NUM_SIGNATURE_BYTES
            ),
            FILE,
            "Cannot parse order from data"
        );

coverage_0x729d531c(0x67b2c7670a51c212cce7a6905866e1877d55717f4755eaac1edc45420485fcda); /* line */ 
        coverage_0x729d531c(0x2dba97d8670d7414cc098812fa271b415bcb795c7d87fc5abbcd2c7ac4d19edb); /* statement */ 
OrderInfo memory orderInfo;
coverage_0x729d531c(0xc21308b3e22a32fe4298750dee53ea6e665f543c64197306f094903dcb97294e); /* line */ 
        coverage_0x729d531c(0x93d22e3f3bf36ae84ce5bc77fbe137b84255d76137addfc7d6a838ac804af881); /* statement */ 
orderInfo.order = abi.decode(data, (Order));
coverage_0x729d531c(0xb4e7f3fba97e729c136367568fa60a63c9eaf3732ef743bd0b38e054ff3c4ffe); /* line */ 
        coverage_0x729d531c(0x1f4ae6452f0db4c4f30ae1d21212461e0a6ac99abbf0fda8692f4006f24e777a); /* statement */ 
orderInfo.orderHash = getOrderHash(orderInfo.order);

coverage_0x729d531c(0x12ac12b0b3ef6bb2ada53d8a4bcdf4e43466f0a48d7f66c06cbc9862d6dc38a4); /* line */ 
        coverage_0x729d531c(0x46d291b06dda99094a28a4428307a3ee9e0487e001de09503339874ac16463a8); /* statement */ 
OrderStatus orderStatus = g_status[orderInfo.orderHash];

        // verify valid signature or is pre-approved
coverage_0x729d531c(0x1bae294564ead39a54aa047c7fbde2ea54cc6cb5daa7ba8a1fac8bc100c27208); /* line */ 
        coverage_0x729d531c(0x5d3d5c94d58cf07e891195e9027aba08f518d4733b7b814b108382b12c3ca332); /* statement */ 
if (orderStatus == OrderStatus.Null) {coverage_0x729d531c(0xa13757746c326a6cfbfc73a8ec83e7f43ccbac8385af814b05d3fe62721ab947); /* branch */ 

coverage_0x729d531c(0x028c49454caa7eb74535f3ea40fc94c0b1d2abbc485685ba01e1e65c91e97c5c); /* line */ 
            coverage_0x729d531c(0x9029bc9bd1d1d4180e7ce46c148bbd53f01e9d5007441e1df16e7263fc423b79); /* statement */ 
bytes memory signature = parseSignature(data);
coverage_0x729d531c(0x136240220913d2fad60d1ef2e8493c2516fb40a21e954376b99361ecba81d409); /* line */ 
            coverage_0x729d531c(0x65ff595b8401449171f5616c819f19edd2efc31f2bb6438530ba27d9084f0f78); /* statement */ 
address signer = TypedSignature.recover(orderInfo.orderHash, signature);
coverage_0x729d531c(0xbe29b25f058316ece8477ce2bb7a4665858048240cccbc173b0c41ce7148660a); /* line */ 
            coverage_0x729d531c(0x00e3ddda76e07f096844ed9a6bff7ba90b2330b59206c693389bc2ddfdaf17ad); /* statement */ 
Require.that(
                orderInfo.order.makerAccountOwner == signer,
                FILE,
                "Order invalid signature",
                orderInfo.orderHash
            );
        } else {coverage_0x729d531c(0xfa30497ee0329c146fda6b0430c375c341bbe1451d715bd000d125bba209fe3b); /* branch */ 

coverage_0x729d531c(0x3a3bbf44e02fc3806902988113cf74fe9d78d5b20897cbdb9d9f87f3817cc011); /* line */ 
            coverage_0x729d531c(0xa198ce522bcb88ba1ed642a88d0664b7a3d0c1ab53bb08d86c07241ef3aa36c2); /* statement */ 
Require.that(
                orderStatus != OrderStatus.Canceled,
                FILE,
                "Order canceled",
                orderInfo.orderHash
            );
coverage_0x729d531c(0xb988f317d4a51c5acdb35a4070fdf4f9e4564f8eafc0658a98f106e4a3ffdc24); /* line */ 
            coverage_0x729d531c(0x98b948543ed6a6efb1145949351074df57f5344cc4fe2e04ad76b0718b71da6b); /* assertPre */ 
coverage_0x729d531c(0x416c371b2555134d53bf1fb5a41a5c001898b37d70997b4cdc7180c99675e8a0); /* statement */ 
assert(orderStatus == OrderStatus.Approved);coverage_0x729d531c(0x093ba702cddb60e48264857570b2b0aa913a748d3d2a9300a72c034495cd8db2); /* assertPost */ 

        }

coverage_0x729d531c(0x571d425c0f3e4f46ea4dc8a9ea4c7080bb034db7975499365600ef93e9b70374); /* line */ 
        coverage_0x729d531c(0x16a0af4df30cb3b2921e197c72938959b807c67e069005aa739ec8d073b2f3c8); /* statement */ 
return orderInfo;
    }

    // ============ Private Parsing Functions ============

    /**
     * Returns the EIP712 hash of an order.
     */
    function getOrderHash(
        Order memory order
    )
        private
        view
        returns (bytes32)
    {coverage_0x729d531c(0x6a37fdc32624293204f910ac490b9ee3ec360e72c882a9a44ab0016e5f5ceb8f); /* function */ 

        // compute the overall signed struct hash
        /* solium-disable-next-line indentation */
coverage_0x729d531c(0x8ba0ea18d643f267b932c834769329b79bd150466d3f2390e0f365f869b52744); /* line */ 
        coverage_0x729d531c(0xee3527e0b3aa10ac1c1787b5c6198065e932333d1b0759a925f2ac52eb59fd19); /* statement */ 
bytes32 structHash = keccak256(abi.encode(
            EIP712_LIMIT_ORDER_STRUCT_SCHEMA_HASH,
            order
        ));

        // compute eip712 compliant hash
        /* solium-disable-next-line indentation */
coverage_0x729d531c(0x14efcb3b2ddf51cf46339b5ef776a0eee49128ce05960a3988b0be3fc1fe197e); /* line */ 
        coverage_0x729d531c(0xf5c1cb7282938a4590d7c522b53a9e257104a931288935f43da1020548e90623); /* statement */ 
return keccak256(abi.encodePacked(
            EIP191_HEADER,
            EIP712_DOMAIN_HASH,
            structHash
        ));
    }

    /**
     * Parses out a signature from call data.
     */
    function parseSignature(
        bytes memory data
    )
        private
        pure
        returns (bytes memory)
    {coverage_0x729d531c(0x93633ef5088b7ebf464bba6f195cc43a0b4922d2602e439d0245fac618730566); /* function */ 

coverage_0x729d531c(0xeaf5fae5649e000bb6936763b5f17269fd225c2a9858d49f9d154833e1299ed1); /* line */ 
        coverage_0x729d531c(0x1bf027ed800aa55b6061eaf3c79d4656755a227c28576a43e0bd5f665be8b88b); /* statement */ 
Require.that(
            data.length == NUM_ORDER_BYTES + NUM_SIGNATURE_BYTES,
            FILE,
            "Cannot parse signature from data"
        );

coverage_0x729d531c(0x3d59be085d63def84be2da232a4ddccacb046adeb2d958a951d1124bab151a9f); /* line */ 
        coverage_0x729d531c(0xb94e72788ff1fc324d7dd4fc73bf969e7dcfbf912c8bd3e4db50e567dd5e1d04); /* statement */ 
bytes memory signature = new bytes(NUM_SIGNATURE_BYTES);

coverage_0x729d531c(0xa0c52735d03ad4fb9499a902f8c7665cd725cc46c90fa2ac0894fdca2af0bf7b); /* line */ 
        coverage_0x729d531c(0x87d5f239b05b820138f4e4002779d2545a990e5e4bb8287509ff3187447208e0); /* statement */ 
uint256 sigOffset = NUM_ORDER_BYTES;
        /* solium-disable-next-line security/no-inline-assembly */
coverage_0x729d531c(0x5bf508cbaf509dc11dc0155284794706c57366a0fd73e38de793fb8afdf9b117); /* line */ 
        assembly {
            let sigStart := add(data, sigOffset)
            mstore(add(signature, 0x020), mload(add(sigStart, 0x20)))
            mstore(add(signature, 0x040), mload(add(sigStart, 0x40)))
            mstore(add(signature, 0x042), mload(add(sigStart, 0x42)))
        }

coverage_0x729d531c(0x53c77139e487f06bf8f93eee7de55e86c9005a73cd702fa3012309125fa163b4); /* line */ 
        coverage_0x729d531c(0xc3c4d80bee96ab82e0ece5755da927b3672272692e159c00d1b502f33e0b20f7); /* statement */ 
return signature;
    }
}
