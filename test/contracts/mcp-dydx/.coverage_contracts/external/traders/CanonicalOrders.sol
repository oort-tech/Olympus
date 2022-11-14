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
import { Monetary } from "../../protocol/lib/Monetary.sol";
import { Require } from "../../protocol/lib/Require.sol";
import { Types } from "../../protocol/lib/Types.sol";
import { OnlySolo } from "../helpers/OnlySolo.sol";
import { TypedSignature } from "../lib/TypedSignature.sol";


/**
 * @title CanonicalOrders
 * @author dYdX
 *
 * Allows for Canonical Orders to be used with dYdX
 */
contract CanonicalOrders is
    Ownable,
    OnlySolo,
    IAutoTrader,
    ICallee
{
function coverage_0xec06f2a9(bytes32 c__0xec06f2a9) public pure {}

    using Math for uint256;
    using SafeMath for uint256;
    using Types for Types.Par;
    using Types for Types.Wei;

    // ============ Constants ============

    bytes32 constant private FILE = "CanonicalOrders";

    // EIP191 header for EIP712 prefix
    bytes2 constant private EIP191_HEADER = 0x1901;

    // EIP712 Domain Name value
    string constant private EIP712_DOMAIN_NAME = "CanonicalOrders";

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

    // Hash of the EIP712 CanonicalOrder struct
    /* solium-disable-next-line indentation */
    bytes32 constant private EIP712_ORDER_STRUCT_SCHEMA_HASH = keccak256(abi.encodePacked(
        "CanonicalOrder(",
        "bytes32 flags,",
        "uint256 baseMarket,",
        "uint256 quoteMarket,",
        "uint256 amount,",
        "uint256 limitPrice,",
        "uint256 triggerPrice,",
        "uint256 limitFee,",
        "address makerAccountOwner,",
        "uint256 makerAccountNumber,",
        "uint256 expiration",
        ")"
    ));

    // Number of bytes in an Order struct plus number of bytes in a FillArgs struct
    uint256 constant private NUM_ORDER_AND_FILL_BYTES = 416;

    // Number of bytes in a typed signature
    uint256 constant private NUM_SIGNATURE_BYTES = 66;

    // The number of decimal places of precision in the price ratio of a triggerPrice
    uint256 constant private PRICE_BASE = 10 ** 18;

    // Bitmasks for the order.flag argument
    bytes32 constant private IS_BUY_FLAG = bytes32(uint256(1));
    bytes32 constant private IS_DECREASE_ONLY_FLAG = bytes32(uint256(1 << 1));
    bytes32 constant private IS_NEGATIVE_FEE_FLAG = bytes32(uint256(1 << 2));

    // ============ Enums ============

    enum OrderStatus {
        Null,
        Approved,
        Canceled
    }

    enum CallFunctionType {
        Approve,
        Cancel,
        SetFillArgs
    }

    // ============ Structs ============

    struct Order {
        bytes32 flags; // salt, negativeFee, decreaseOnly, isBuy
        uint256 baseMarket;
        uint256 quoteMarket;
        uint256 amount;
        uint256 limitPrice;
        uint256 triggerPrice;
        uint256 limitFee;
        address makerAccountOwner;
        uint256 makerAccountNumber;
        uint256 expiration;
    }

    struct FillArgs {
        uint256 price;
        uint128 fee;
        bool isNegativeFee;
    }

    struct OrderInfo {
        Order order;
        FillArgs fill;
        bytes32 orderHash;
    }

    struct OrderQueryOutput {
        OrderStatus orderStatus;
        uint256 filledAmount;
    }

    // ============ Events ============

    event LogContractStatusSet(
        bool operational
    );

    event LogTakerSet(
        address taker
    );

    event LogCanonicalOrderCanceled(
        bytes32 indexed orderHash,
        address indexed canceler,
        uint256 baseMarket,
        uint256 quoteMarket
    );

    event LogCanonicalOrderApproved(
        bytes32 indexed orderHash,
        address indexed approver,
        uint256 baseMarket,
        uint256 quoteMarket
    );

    event LogCanonicalOrderFilled(
        bytes32 indexed orderHash,
        address indexed orderMaker,
        uint256 fillAmount,
        uint256 triggerPrice,
        bytes32 orderFlags,
        FillArgs fill
    );

    // ============ Immutable Storage ============

    // Hash of the EIP712 Domain Separator data
    bytes32 public EIP712_DOMAIN_HASH;

    // ============ Mutable Storage ============

    // true if this contract can process orders
    bool public g_isOperational;

    // order hash => filled amount (in baseAmount)
    mapping (bytes32 => uint256) public g_filledAmount;

    // order hash => status
    mapping (bytes32 => OrderStatus) public g_status;

    // stored fillArgs
    FillArgs public g_fillArgs;

    // required taker address
    address public g_taker;

    // ============ Constructor ============

    constructor (
        address soloMargin,
        address taker,
        uint256 chainId
    )
        public
        OnlySolo(soloMargin)
    {coverage_0xec06f2a9(0xf1e4159596b0e51e9951707ddbf3774a6f6c757e4fcf2060055cab9f1e0c7a97); /* function */ 

coverage_0xec06f2a9(0xe874f5b8a9f60cf9b08be2d607892d4cdaa019ace415145db6695cc0346e240f); /* line */ 
        coverage_0xec06f2a9(0x0c4390763e93db1fa48aa7d6409a810e0d9d22d537cf0fc232e54b5db3b5cff5); /* statement */ 
g_isOperational = true;
coverage_0xec06f2a9(0x7b641b6cea8f7b191a36a9a141f02fd77a47b3f20770fda2697ac01f6f03c071); /* line */ 
        coverage_0xec06f2a9(0x6272fc110fae62559fc3b07bc1381bbbfb1d3347dbdeaefd0a0d9c160cd077be); /* statement */ 
g_taker = taker;

        /* solium-disable-next-line indentation */
coverage_0xec06f2a9(0x6e8536ab4e2e6b61fe572c2b532c7f82e727a52d022df53e5b3230bc711f4f81); /* line */ 
        coverage_0xec06f2a9(0xc68b2155afad38c6ff37cd0f4552e30161bcf713710fa4acdcbfaf397fe6e0a6); /* statement */ 
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
    {coverage_0xec06f2a9(0xefd89059cfcee65bc7ee588e38fa8c270764b9c7d46b00553e994290c0ce5bb6); /* function */ 

coverage_0xec06f2a9(0xf42ccfff99e1e105f6aa2517ebbc4ba53cff91ea64d6e01890429939a8ce217a); /* line */ 
        coverage_0xec06f2a9(0x31e5a52da9dc3261f4e406d4d04f4f11056b4938d9dda22029d07d6345d81570); /* statement */ 
g_isOperational = false;
coverage_0xec06f2a9(0x0336bc20d97eda9a611bf08719f5368cca3d2efb036907a102c5fc2c27b1b8a2); /* line */ 
        coverage_0xec06f2a9(0x4ed4462053b9769febf605cfe7e069c260d250ac48055c2d1ec61b3e1c6d41d3); /* statement */ 
emit LogContractStatusSet(false);
    }

    /**
     * The owner can start back up the exchange.
     */
    function startUp()
        external
        onlyOwner
    {coverage_0xec06f2a9(0xacc2ae55265da556512eb7c891f97e0821aee188db62abec9eb963bda9954f91); /* function */ 

coverage_0xec06f2a9(0xffaab47ac4fbd88dbd20019097cd20fedbc1e613432866ff7b338900d39f7f8f); /* line */ 
        coverage_0xec06f2a9(0xfd4601711a7763aa43647bb4b6b71f317e473f28fc89b47083c9aa34fb1c193a); /* statement */ 
g_isOperational = true;
coverage_0xec06f2a9(0xd9a41646a1100f53529c7ece5e22481c91ec0199341d3876cda5b8f3868be6d3); /* line */ 
        coverage_0xec06f2a9(0x0c45033a1453868a6b99628e1f9644accd15301317e48f973d6a742e6718b874); /* statement */ 
emit LogContractStatusSet(true);
    }

    /**
     * The owner can set the taker address.
     */
    function setTakerAddress(
        address taker
    )
        external
        onlyOwner
    {coverage_0xec06f2a9(0x19b35ec2fdb80e133d737b573b97fbbae861c5e1d572ef8be502d0e65cdb62b0); /* function */ 

coverage_0xec06f2a9(0x82a23d7e2d6f00676c7a65293aded901be57a481155cf76f21897fc3931548cc); /* line */ 
        coverage_0xec06f2a9(0x55140d8c7372023fdb2a0439f01fad5032ab25c4db937d81156c30a4fa8155a0); /* statement */ 
g_taker = taker;
coverage_0xec06f2a9(0x3479308a530b04db0daaf9ea531c2d1a9d26dcae5b505d825b07c96953346f1d); /* line */ 
        coverage_0xec06f2a9(0xa97bad7ada6848fc91bbfc50f937c6485fe0823516c5909b7d6d2a4e31a16702); /* statement */ 
emit LogTakerSet(taker);
    }

    // ============ External Functions ============

    /**
     * Cancels an order.
     *
     * @param  order  The order to cancel
     */
    function cancelOrder(
        Order memory order
    )
        public
    {coverage_0xec06f2a9(0xb8969b423a9f17b10a68502005692e49574785b619564fd8c6e2ae745840e710); /* function */ 

coverage_0xec06f2a9(0x5e124b3a72e06e3f5c2b7f12467b2e848e83049260bd5daeca6d2ef17ab607b9); /* line */ 
        coverage_0xec06f2a9(0x1ebf8a2c0416cb851c0c5ee63cbc311c6c6605d476d9df1512442f642d9d91fa); /* statement */ 
cancelOrderInternal(msg.sender, order);
    }

    /**
     * Approves an order. Cannot already be canceled.
     *
     * @param  order  The order to approve
     */
    function approveOrder(
        Order memory order
    )
        public
    {coverage_0xec06f2a9(0xdb15b2bb669b2e22ba687a6fc1b0ffef9cf737c1d97eaf2ad86a44747f6ee5ed); /* function */ 

coverage_0xec06f2a9(0xe1e47848d12d9701f38efd018ab5da7ae2b3a4b75e627bbb6efc22acd6ff9eed); /* line */ 
        coverage_0xec06f2a9(0xdd2b51021f67e54929bb0e537cc64caa9fe922d83c7b478be67e48968242b6ec); /* statement */ 
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
     * @param  oldInputPar     The par balance of the makerAccount for inputMarketId pre-trade
     * @param  newInputPar     The par balance of the makerAccount for inputMarketId post-trade
     * @param  inputWei        The change in token amount for the makerAccount for the inputMarketId
     * @param  data            Arbitrary data passed in by the trader
     * @return                 The AssetAmount for the makerAccount for the outputMarketId
     */
    function getTradeCost(
        uint256 inputMarketId,
        uint256 outputMarketId,
        Account.Info memory makerAccount,
        Account.Info memory takerAccount,
        Types.Par memory oldInputPar,
        Types.Par memory newInputPar,
        Types.Wei memory inputWei,
        bytes memory data
    )
        public
        onlySolo(msg.sender)
        returns (Types.AssetAmount memory)
    {coverage_0xec06f2a9(0xeb64edba61ddab6cb99cf5ca337b90b40c891e3f986161ae59705995323a244a); /* function */ 

coverage_0xec06f2a9(0xc7b252b180ed82a5757b74996792bdd515e0ff45f9c9e5fd66b5ac3031ac7665); /* line */ 
        coverage_0xec06f2a9(0x0bc968576cd7fd71ebea98085f5523aaf7d97b8ae2554d515df809b5606f0bf5); /* statement */ 
Require.that(
            g_isOperational,
            FILE,
            "Contract is not operational"
        );

coverage_0xec06f2a9(0xc544fa9a55b8e9f7774bfd4f399ce698d28a684785d04ed8b9b1c932b72dcedf); /* line */ 
        coverage_0xec06f2a9(0x115bfe1e035cd018937400f45f0efa5fd1b034a3b253f641f37d0e61170367d0); /* statement */ 
OrderInfo memory orderInfo = getOrderInfo(data);

coverage_0xec06f2a9(0xce059d971f2e7179b7dff70f2e8191317a7ebee71fe0af4519e2ed59e47f3608); /* line */ 
        coverage_0xec06f2a9(0x038862161b5f5ecae5188b0f0db875c8287155ad3e5c98847804d663197c9493); /* statement */ 
verifySignature(orderInfo, data);

coverage_0xec06f2a9(0xd8d199ba2130920432b7a6f108b8a3af8d455ece2d5390e85d2ab960ab15910a); /* line */ 
        coverage_0xec06f2a9(0xac038381468cb4c603f58f2ea920531c3cc620a775e6c2df35b043e365a8ed85); /* statement */ 
verifyOrderInfo(
            orderInfo,
            makerAccount,
            takerAccount,
            inputMarketId,
            outputMarketId,
            inputWei
        );

coverage_0xec06f2a9(0x60ea9f43eb231d9717a2cf7f1702ff04b05ef5c9c5119c4d4185901791fe1e12); /* line */ 
        coverage_0xec06f2a9(0xc8ef922bc7002dbfae9b17f06dad6b933c1889fdba0dfbb42b5e601f5bc7420b); /* statement */ 
Types.AssetAmount memory assetAmount = getOutputAssetAmount(
            inputMarketId,
            outputMarketId,
            inputWei,
            orderInfo
        );

coverage_0xec06f2a9(0xddcabb913446d8ec5fab8ca53bb0122288d817c8aea6b3ff631c42d60d93e19e); /* line */ 
        coverage_0xec06f2a9(0x60bba04827a38d781d2ae5a23a292cda117b77860d59371b743ab186a69f9c8f); /* statement */ 
if (isDecreaseOnly(orderInfo.order)) {coverage_0xec06f2a9(0x3f5664e3ebeea07829d979e05b56dead14d24e136af2abf199cb4929b25e6b8b); /* branch */ 

coverage_0xec06f2a9(0x3c8e4599685c02a35d6b631787aa7affec882c1b15598c1dfd1655a21355cd46); /* line */ 
            coverage_0xec06f2a9(0xa35d26b7efa87dd311445cfa8916f80062911322ded44dfa5e693b3ef15f6da2); /* statement */ 
verifyDecreaseOnly(
                oldInputPar,
                newInputPar,
                assetAmount,
                makerAccount,
                outputMarketId
            );
        }else { coverage_0xec06f2a9(0xce238f58b83649dbbae6495ef1f25470a0c77295ae8991df8fc9279c5ba94247); /* branch */ 
}

coverage_0xec06f2a9(0x6bdbb121e0a8fd78b842b3ebaac13f4386d93ad137045795937c5bf8e7a6300b); /* line */ 
        coverage_0xec06f2a9(0x9103df414a0f7e8513caf697603197438734b918d88d5f1c9c75416468dec5e9); /* statement */ 
return assetAmount;
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
    {coverage_0xec06f2a9(0x7c22576661559189040d65e6adb00b6968e7743a72938da1658d5568a1cb80aa); /* function */ 

coverage_0xec06f2a9(0x41322fcc267e800df8e2bf1d08fc42f22acdc9bf6a9ded8bcdf8dc006c90cf9e); /* line */ 
        coverage_0xec06f2a9(0x6c10c8ade45f102445275b24f64ee1d1fbb119e5544b9b8527e1ad35d39e9e83); /* statement */ 
CallFunctionType cft = abi.decode(data, (CallFunctionType));

coverage_0xec06f2a9(0x7dc5b10f279e35c8dc9cd7a728af22000a3904adc054f52a53c387622188fa9b); /* line */ 
        coverage_0xec06f2a9(0x0b3e98c2adcd6a03dcf95e4cdad4845a5f277e5e673364e8bf74a21b6d123344); /* statement */ 
if (cft == CallFunctionType.SetFillArgs) {coverage_0xec06f2a9(0x19edc9bc81e2b46f8d0e7da3910ae525a00c4edfb09e4e80ad1c651f006b4b70); /* branch */ 

coverage_0xec06f2a9(0xe2c918e4d4a77a58711935b238f0eebeeb30c9ff4f634f77a93c0cde63358533); /* line */ 
            coverage_0xec06f2a9(0x272f4ed87ac5779a4de70d36881cc6a3af2e66b3a5993858dd2cb2fab68860be); /* statement */ 
FillArgs memory fillArgs;
coverage_0xec06f2a9(0xff4981ec15e1ed7693cd023bce399b6a4c19febb907d438412546023c2c199e1); /* line */ 
            coverage_0xec06f2a9(0x20838841765c27f0e080790f53e0cb3f9c6eb08690c629249d0cfb339fc70e98); /* statement */ 
(cft, fillArgs) = abi.decode(data, (CallFunctionType, FillArgs));
coverage_0xec06f2a9(0x651835aa2f9bde3fb8a291b111231ce75d82ff3e12e69c8208462e06ef6f8c8e); /* line */ 
            coverage_0xec06f2a9(0x74d685f58944b0235617667e3067e1dbc1b40b0cd5467adc31bbaf6cec9c6186); /* statement */ 
g_fillArgs = fillArgs;
        } else {coverage_0xec06f2a9(0x3eae7c5de3b508cc6488e0b0729472e6fbcf8395190d0bd7a97555d3226a7d74); /* branch */ 

coverage_0xec06f2a9(0xd767954beae172760fe1118728e16091af3360ed2578d2306f170ae9c9d66737); /* line */ 
            coverage_0xec06f2a9(0xd714e1fd118a61b68b474d5716995014bbd217bd77bd2e62c43c8df3041b328c); /* statement */ 
Order memory order;
coverage_0xec06f2a9(0x9f6e0426102b2ebbb1877945f0852741907e31b51f541282de2994afbaafbfb7); /* line */ 
            coverage_0xec06f2a9(0x2cabaed7335c8f6075644d46bd48e7d00fbc015485189940527827078d3a5802); /* statement */ 
(cft, order) = abi.decode(data, (CallFunctionType, Order));
coverage_0xec06f2a9(0x4ce772ac9e7fa6f3b42ea62120da2053496ab4e25916966d5e884cb76cf45875); /* line */ 
            coverage_0xec06f2a9(0xffb73e058c3529a0240b13a8c9930443cee9d0ba5062271549337bdd89a3b294); /* statement */ 
if (cft == CallFunctionType.Approve) {coverage_0xec06f2a9(0x6d3a5c23092c56752420f88602ddd1817011e877406c6fd9e04965fc5b5b6c6f); /* branch */ 

coverage_0xec06f2a9(0x6a2920b27f2aab6cf1a3988be54c6e754ef00644a7f6fc58fad36e34c6397163); /* line */ 
                coverage_0xec06f2a9(0xa1b239c889d9e2739d9bb09a3472532134375d3b722d97c4d8a67deda1f4b907); /* statement */ 
approveOrderInternal(accountInfo.owner, order);
            } else {coverage_0xec06f2a9(0x9efbc837455e10505af04b7723bcbd6addf3833dac36b66384802cecc8abf697); /* branch */ 

coverage_0xec06f2a9(0x00549fd14d577d9b0bb4fa957837de7fd44006e4fb7e2cb8ebe83f7521e04442); /* line */ 
                coverage_0xec06f2a9(0xfa135e8bfd12594dca7fb1ed358b42aace85fef5a2e9d0d31bec50210d2bcfd4); /* assertPre */ 
coverage_0xec06f2a9(0x9fc1a86e374d0a9562223a52f9f6669c7fdf5a5df796bb61f6c5ab5b3ce3fc84); /* statement */ 
assert(cft == CallFunctionType.Cancel);coverage_0xec06f2a9(0x36e5f0227bcead2f4b31419f8c75261dc58596cb4e040368151a01b1bc7035ba); /* assertPost */ 

coverage_0xec06f2a9(0x13427b4aeefdd378538b3f2d864e5c1d2092466e1e5372e25ee873ebb1d834a9); /* line */ 
                coverage_0xec06f2a9(0x92791c45f19c2f995ff85ec70226ab3416f08f6d0ddbcb5281bb657bdac5ac01); /* statement */ 
cancelOrderInternal(accountInfo.owner, order);
            }
        }
    }

    // ============ Getters ============

    /**
     * Returns the status and the filled amount of several orders.
     */
    function getOrderStates(
        bytes32[] memory orderHashes
    )
        public
        view
        returns(OrderQueryOutput[] memory)
    {coverage_0xec06f2a9(0x1efa0d3d7bd86a0f7779ab499b58a4bb21ed583b07408433c0ae2d9cc3ddd5a4); /* function */ 

coverage_0xec06f2a9(0xac1477ea36c3ac52d6774fc553f8260288a678cc511d633df095a010ca2e2593); /* line */ 
        coverage_0xec06f2a9(0x711d844f59f79309e2f58bb2d7e24c5c2e6d0a2649cdeec8fc9bdb110cc5be7b); /* statement */ 
uint256 numOrders = orderHashes.length;
coverage_0xec06f2a9(0x79dc902102201e7f56ab824223507916a09dd84bc872e959cc84184b3289faaf); /* line */ 
        coverage_0xec06f2a9(0x90dad9d77685674a279fc4807241107ea545898144bd8b9b47983c2e75000bf3); /* statement */ 
OrderQueryOutput[] memory output = new OrderQueryOutput[](numOrders);

        // for each order
coverage_0xec06f2a9(0xd6904d1b81a3583e8c483b67f9889fccf09a0a78541512fdea717b1c33644d50); /* line */ 
        coverage_0xec06f2a9(0x5930eb79630be086bb980f5c3b8a3c15a88a5ef3918b60604f907a2812ecccfc); /* statement */ 
for (uint256 i = 0; i < numOrders; i++) {
coverage_0xec06f2a9(0x858a0a69745c6b9e6852c04c558eb9c7e28bb74deb10d072194f57ee9bc10040); /* line */ 
            coverage_0xec06f2a9(0x52f984403b780b8d06095eef170c3bc8783f0067631f236ba0b3b78d0a38a11e); /* statement */ 
bytes32 orderHash = orderHashes[i];
coverage_0xec06f2a9(0x3baa6597f0ad07a9ae79e0c21ba631c6651768a33112f45770f9fb5efc12c040); /* line */ 
            coverage_0xec06f2a9(0x66942baff0817465ded9291177c3701fdc89a4d8622c8df1fba77174d4789d04); /* statement */ 
output[i] = OrderQueryOutput({
                orderStatus: g_status[orderHash],
                filledAmount: g_filledAmount[orderHash]
            });
        }
coverage_0xec06f2a9(0xfa208ce5cceb5050b99e2f4992b662815d55fd5ed30e6b2cb2ee292b80501268); /* line */ 
        coverage_0xec06f2a9(0x4cb7009030420b4328ab485192a781f5504fab618042aae8e45c3f78f0227256); /* statement */ 
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
    {coverage_0xec06f2a9(0x513838ef424a41edb865be84d9b0506da147a0071f9783e6154c3e4e22159a09); /* function */ 

coverage_0xec06f2a9(0x978f998df07d55c76265e48602f1d2976c75ed5a592db2e8ba20f37c1ae0a466); /* line */ 
        coverage_0xec06f2a9(0x6db36939c14cae6a4be7ab4d5e5093ed4c11bcf0d891c0016c27ae60a881a687); /* statement */ 
Require.that(
            canceler == order.makerAccountOwner,
            FILE,
            "Canceler must be maker"
        );
coverage_0xec06f2a9(0x9ee6aaaa23adbeacdf3d7699263c7b58e525cb3e85cd241a5fe7264b091436c3); /* line */ 
        coverage_0xec06f2a9(0xfc87dc8e9341b6f19c0c089e5cc15d79630af88a24b8a57980f956ab1b4e50ca); /* statement */ 
bytes32 orderHash = getOrderHash(order);
coverage_0xec06f2a9(0x8d6f1e344337ca040e9c0cd2a89ed610be78bb1b7d943cd71785363ecfb82051); /* line */ 
        coverage_0xec06f2a9(0x1f0c864f596db6044dd2cc2bf7605f66051a978e8f596480e3ef1dee708c7f15); /* statement */ 
g_status[orderHash] = OrderStatus.Canceled;
coverage_0xec06f2a9(0x504baca8bcf1eac4fa4a74a2762360c1679fcbf2fa4e2f2e72ceb4f232a7e5fa); /* line */ 
        coverage_0xec06f2a9(0x780e1ef9feae25ee9ccec48ba6bf1a2d2574ec3915fa5ebbe150007e4a05762d); /* statement */ 
emit LogCanonicalOrderCanceled(
            orderHash,
            canceler,
            order.baseMarket,
            order.quoteMarket
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
    {coverage_0xec06f2a9(0x410a2c0d89970758e2d9acc8ef51170860bb887083c49a06e37676098db8f9a7); /* function */ 

coverage_0xec06f2a9(0x2da3e1032b28dbafa9b4bd5f240b0a0ae4b5757e5aa9309c4cec27a1fc14cb9e); /* line */ 
        coverage_0xec06f2a9(0xbd92c9c467f7e3488d77b52f63c9eb6bb64c0365b9d26c392f5432fbcb2f7568); /* statement */ 
Require.that(
            approver == order.makerAccountOwner,
            FILE,
            "Approver must be maker"
        );
coverage_0xec06f2a9(0xe748e1f90488b0cb30481f3cdb99e9e7809287ec09de28e4a7079212d1503c33); /* line */ 
        coverage_0xec06f2a9(0x3a93dd38ac935c6b76c5ad3e3fa1724439898645e8cd799b2f0c5f294d485bce); /* statement */ 
bytes32 orderHash = getOrderHash(order);
coverage_0xec06f2a9(0xf497d0bc211560b4ac58e8318e715ee504d3fc0266120bde3ce7dd5207108eb6); /* line */ 
        coverage_0xec06f2a9(0x0e63c9af776c4a5f3b22968f92ac828855d5f5749854f6b826d99f61da24315a); /* statement */ 
Require.that(
            g_status[orderHash] != OrderStatus.Canceled,
            FILE,
            "Cannot approve canceled order",
            orderHash
        );
coverage_0xec06f2a9(0x09ea2c1f715cda454bca03b1a72a718172a15ced29587349c2e60b2979eab207); /* line */ 
        coverage_0xec06f2a9(0x396c28803b413dfa9fc60d98804a132b22d8f8fcced0d029e74b8c34d02a9b55); /* statement */ 
g_status[orderHash] = OrderStatus.Approved;
coverage_0xec06f2a9(0x0c51a2e4016ca397f10a16ae8d8e1e5f9f8ceedd8ea154ca3aa39e0e531a108e); /* line */ 
        coverage_0xec06f2a9(0x223156e598fce2804093898e0ef0e7fe81eade777ed50b17d493f2ef72764296); /* statement */ 
emit LogCanonicalOrderApproved(
            orderHash,
            approver,
            order.baseMarket,
            order.quoteMarket
        );
    }

    // ============ Private Helper Functions ============

    /**
     * Parses the order, verifies that it is not expired or canceled, and verifies the signature.
     */
    function getOrderInfo(
        bytes memory data
    )
        private
        returns (OrderInfo memory)
    {coverage_0xec06f2a9(0x0e44e70482c8b9f8f10fe6abb2d489088683fd2c0975b473b5435e7a9eadb4fa); /* function */ 

coverage_0xec06f2a9(0x9077c21ae0b971a4360ffff5b5d3ce8a6973812a8a8bac55527a103fe1e4326d); /* line */ 
        coverage_0xec06f2a9(0x4a28f9ca3ef71b006240a28f1eb39c0e33a14b8779d84c9836f1f6f7bf776411); /* statement */ 
Require.that(
            (
                data.length == NUM_ORDER_AND_FILL_BYTES ||
                data.length == NUM_ORDER_AND_FILL_BYTES + NUM_SIGNATURE_BYTES
            ),
            FILE,
            "Cannot parse order from data"
        );

        // load orderInfo from calldata
coverage_0xec06f2a9(0x6068a17a6821f2cbd1e9f0c370be91c16ca1dd8e1cc1c6fbb4935a6c9eec06a3); /* line */ 
        coverage_0xec06f2a9(0xc1ab20fc02469141166f2b1b9dc5f2eca987b6ac03bf40830582425bc88bd5d8); /* statement */ 
OrderInfo memory orderInfo;
coverage_0xec06f2a9(0x12d301f3865df4f5fe8882115bf4ac3482af25364e06fc76e4f641698252854d); /* line */ 
        coverage_0xec06f2a9(0x73b06d3c3be09cdc329a767557383347cf71889521eba3db3539e9af01a26247); /* statement */ 
(
            orderInfo.order,
            orderInfo.fill
        ) = abi.decode(data, (Order, FillArgs));

        // load fillArgs from storage if price is zero
coverage_0xec06f2a9(0x79fe141dbc55edbd5e309b9c17295beb68f7ed721c2c11fba55eb9a7f4ab6b50); /* line */ 
        coverage_0xec06f2a9(0xd55fc20af397a0f697478d0c9214528530ac8420d712021cd5b4520617914e24); /* statement */ 
if (orderInfo.fill.price == 0) {coverage_0xec06f2a9(0x091112a0d0b3a12c4f541da7b1c8940ba56647e8476ad91ed6d86e6f9bf86392); /* branch */ 

coverage_0xec06f2a9(0x52241263ca56ce98f951d4d669ad979d2bdc5a29e734794249de21a38da2477b); /* line */ 
            coverage_0xec06f2a9(0x60745348a6c8127024732afee89dcfba57b6a3114a91aa9fb8756405c46a48dc); /* statement */ 
orderInfo.fill = g_fillArgs;
coverage_0xec06f2a9(0x37ab7c2eb023788b5d4bc33ab0f0b0149d580727988c78127056e947d9610b8b); /* line */ 
            coverage_0xec06f2a9(0x495a78032ebc21c69ff35b658e86a4546fa2233d9efe163c8bd3ed294ab50f4e); /* statement */ 
g_fillArgs = FillArgs({
                price: 0,
                fee: 0,
                isNegativeFee: false
            });
        }else { coverage_0xec06f2a9(0x43fe5cac3b9a78027f9d570b49f19907823a9785cba8b9208bf160086895c8ab); /* branch */ 
}
coverage_0xec06f2a9(0x060af1dfa2e42f05dbebc0c2d2857a1f565ea33fe0073dd3cd6c0c72e8ef4269); /* line */ 
        coverage_0xec06f2a9(0xbc070a7c247e49bcb8df7fc9c3cd687b23f9011082170fbb9d43a80e25b4c148); /* statement */ 
Require.that(
            orderInfo.fill.price != 0,
            FILE,
            "FillArgs loaded price is zero"
        );

coverage_0xec06f2a9(0x81c92fabbd8f21136000d1968751d32f88c04f5222c28257e8988d99c3347292); /* line */ 
        coverage_0xec06f2a9(0x27fdbe2ece886140596eb49788a86a72a2b20568baa2d762a8991d64bebdd05f); /* statement */ 
orderInfo.orderHash = getOrderHash(orderInfo.order);

coverage_0xec06f2a9(0x78794f6862d874d2dd509e42768c25b793fc494b5471bf8e3ea12f5239843c00); /* line */ 
        coverage_0xec06f2a9(0x1b48320e063506d30bb7bb0e853e58ff8b0d5e496715b8feaa27051c362c2c50); /* statement */ 
return orderInfo;
    }

    function verifySignature(
        OrderInfo memory orderInfo,
        bytes memory data
    )
        private
        view
    {coverage_0xec06f2a9(0x24fb23ba2a63ce35f80e8ff58f03f5f6d9993a5fc03ceba4acd02e3293954f6f); /* function */ 

coverage_0xec06f2a9(0x7250ce000f73b337d622ba77a41a0bb2ad0a39f65c1ab339d408de8240787524); /* line */ 
        coverage_0xec06f2a9(0x6cbe066e8ead9bc2d91b1f95939cb3baff915f9e255d380a42c46300c305d573); /* statement */ 
OrderStatus orderStatus = g_status[orderInfo.orderHash];

        // verify valid signature or is pre-approved
coverage_0xec06f2a9(0xf465dcbc5e5964bd1b8d09ecee1fdc904378460ca1075721ac86de436e716099); /* line */ 
        coverage_0xec06f2a9(0xbfbb096ab3c73f31655f86c54d3ccc0bef1366a7042ab85e683871b1c98bbbcf); /* statement */ 
if (orderStatus == OrderStatus.Null) {coverage_0xec06f2a9(0x492c29b1ec17520edede033ba6cbe9aac4ad1effd3db0f1ed86bca281a15a33e); /* branch */ 

coverage_0xec06f2a9(0x1b8b5624468f58fba9f89864d75780d740c9f4a509f1c8e2db28b10289f257c9); /* line */ 
            coverage_0xec06f2a9(0x17340f6300ca3e7699e19b0f94720dc3865e474067a13b5d716c0df62ec58187); /* statement */ 
bytes memory signature = parseSignature(data);
coverage_0xec06f2a9(0x65dcb8fb68efa9873c03a0be69df074e5f496371a9ffe50e6ecb6f1cb2e0b6b1); /* line */ 
            coverage_0xec06f2a9(0x6dda08a38968b90a910962e2baa3156bc599095737b13c7fe8bbc9180675980a); /* statement */ 
address signer = TypedSignature.recover(orderInfo.orderHash, signature);
coverage_0xec06f2a9(0x4bfc3160183c12412e74caef092e177f8da67a9e1e34c25fd5bad4b9673926f8); /* line */ 
            coverage_0xec06f2a9(0x1bf1bd3a05df60c64dcaf562181a1613c2d45f1494d3eb343d49263fa118e7b6); /* statement */ 
Require.that(
                orderInfo.order.makerAccountOwner == signer,
                FILE,
                "Order invalid signature",
                orderInfo.orderHash
            );
        } else {coverage_0xec06f2a9(0x24aad73439f3451523aa94ec3873f874f335f915f9c81ed700d44e16141c2019); /* branch */ 

coverage_0xec06f2a9(0x19ed93c8b6c96e960f265ea26d49c9536932db13ad0123231713f6c38bd5bd18); /* line */ 
            coverage_0xec06f2a9(0x3dd119ddeb55159cf4e45eb6aa9ea29ed0fe3f0a9dbb75990c4cc7048315966c); /* statement */ 
Require.that(
                orderStatus != OrderStatus.Canceled,
                FILE,
                "Order canceled",
                orderInfo.orderHash
            );
coverage_0xec06f2a9(0x782403d0eedbee219d43b6f6e28edbe7ffbf077c81ecf245fbb544a2ffa59f81); /* line */ 
            coverage_0xec06f2a9(0x5a85d380347e4b9c941ea47e07c8e354bd7c142e28cd389b32e1633e6fcf7507); /* assertPre */ 
coverage_0xec06f2a9(0x515d226b4b01a3b666ab714433b16d664341f895771d7894855b15588a94ead8); /* statement */ 
assert(orderStatus == OrderStatus.Approved);coverage_0xec06f2a9(0xc4daf4aae1309ddb1bd911767cd65d7734113b902f691a0db218afcd635c97ff); /* assertPost */ 

        }
    }

    /**
     * Verifies that the order is still fillable for the particular accounts and markets specified.
     */
    function verifyOrderInfo(
        OrderInfo memory orderInfo,
        Account.Info memory makerAccount,
        Account.Info memory takerAccount,
        uint256 inputMarketId,
        uint256 outputMarketId,
        Types.Wei memory inputWei
    )
        private
        view
    {coverage_0xec06f2a9(0xc224f228c9bcecf4d26608eca7094b8354b0dd1a3e5af0ce35a5b9d0379eda14); /* function */ 

        // verify fill price
coverage_0xec06f2a9(0x79fd48bcd42da81760a7d9af9af3c47101413d8211f39a64c50ff72d70ae9604); /* line */ 
        coverage_0xec06f2a9(0x2c858393dd88855fd8af62ec979bba1eac4708e1de5b384cddf11da6c7fb298b); /* statement */ 
FillArgs memory fill = orderInfo.fill;
coverage_0xec06f2a9(0xb13c1087062610a654e3f37f83b27143efae4b89a93cc8a0bd6093851f31d59c); /* line */ 
        coverage_0xec06f2a9(0x80680e66a62a2fe0ab7fc405a50a65423ab05fa3d416c492d3876218634c7729); /* statement */ 
bool validPrice = isBuy(orderInfo.order)
            ? fill.price <= orderInfo.order.limitPrice
            : fill.price >= orderInfo.order.limitPrice;
coverage_0xec06f2a9(0xdaa2ff25c1e813e983faca4f3554216bc8f62996f1ae4633a4af20b07fbe71f7); /* line */ 
        coverage_0xec06f2a9(0xea859e9e3c17fc5a297e30bee2b58697001f4812d5c927674ef3b4ce246666f7); /* statement */ 
Require.that(
            validPrice,
            FILE,
            "Fill invalid price"
        );

        // verify fill fee
coverage_0xec06f2a9(0xd43cdfa8ff1cdf597d5b770822e1563075cd91337f7544b3115949b4bcd1037b); /* line */ 
        coverage_0xec06f2a9(0x947b531b635a0de650ca91ff1d277d45a48c478b7fb68dc92e36baab63222485); /* statement */ 
bool validFee = isNegativeLimitFee(orderInfo.order)
            ? (fill.fee >= orderInfo.order.limitFee) && fill.isNegativeFee
            : (fill.fee <= orderInfo.order.limitFee) || fill.isNegativeFee;
coverage_0xec06f2a9(0x707bfb864e3809568ed9014a9f4cbe4b4e3f949d14b9a83c6f9010f18f41e404); /* line */ 
        coverage_0xec06f2a9(0xeaaef57726558d3f04a266fad028609086da0f789338372754a41882520106d5); /* statement */ 
Require.that(
            validFee,
            FILE,
            "Fill invalid fee"
        );

        // verify triggerPrice
coverage_0xec06f2a9(0x2c36240d0ed3e4e763a90d56703db812f7a6a0cf61731cc2625cf8492fa3bcfd); /* line */ 
        coverage_0xec06f2a9(0x08ee322ee9d9b08b05ab072469380b5051248904a9a76e0d70a30d6f52be1ba3); /* statement */ 
if (orderInfo.order.triggerPrice > 0) {coverage_0xec06f2a9(0xb39522a6fa34f667de288fe176c8fe66e8b513d2cc01339439f8a2ca5c3c489a); /* branch */ 

coverage_0xec06f2a9(0xa96da854c70c6e7480cc669c7e67bf91facc8ab7e4237691140a9548807262c0); /* line */ 
            coverage_0xec06f2a9(0xe605540c13c462624758c113720a7466148cef9d40206447740371b6e17328b9); /* statement */ 
uint256 currentPrice = getCurrentPrice(
                orderInfo.order.baseMarket,
                orderInfo.order.quoteMarket
            );
coverage_0xec06f2a9(0xdf66ad95d57e036478730c08345e570cc1d8593a22a4f125e37fc08f6d8693ed); /* line */ 
            coverage_0xec06f2a9(0xaeff9a2023405519aeb26535ce71e1395c695dc34016a459b1619aa5fbda9fd9); /* statement */ 
Require.that(
                isBuy(orderInfo.order)
                    ? currentPrice >= orderInfo.order.triggerPrice
                    : currentPrice <= orderInfo.order.triggerPrice,
                FILE,
                "Order triggerPrice not triggered",
                currentPrice
            );
        }else { coverage_0xec06f2a9(0xda14f90ad06ce45591a95e1bbc6d0af1dc330612c9e8544143420059f5d2ba00); /* branch */ 
}

        // verify expriy
coverage_0xec06f2a9(0x506b6eea61b9963a7af63dc8ecd683f2f003446a8a441c6efc90273b06099a0b); /* line */ 
        coverage_0xec06f2a9(0xc4abf222bb0e25629f71eb0c0fd211d6c35ecd17e819dc34ec7769e55b0d78b1); /* statement */ 
Require.that(
            orderInfo.order.expiration == 0 || orderInfo.order.expiration >= block.timestamp,
            FILE,
            "Order expired",
            orderInfo.orderHash
        );

        // verify maker
coverage_0xec06f2a9(0x6f1fc1a8c47ebab4d1581d0131ead511b762d4125f5f590f21f781e66488e543); /* line */ 
        coverage_0xec06f2a9(0xa39ba3fcfd056186c978207ae7c1f997e2c4b876e5f2a24f078bd8d0bf9ba7e9); /* statement */ 
Require.that(
            makerAccount.owner == orderInfo.order.makerAccountOwner &&
            makerAccount.number == orderInfo.order.makerAccountNumber,
            FILE,
            "Order maker account mismatch",
            orderInfo.orderHash
        );

        // verify taker
coverage_0xec06f2a9(0x3ad419559d7b245a579df113a3d6beff9876a3e9cf42b7dcde9925310907189d); /* line */ 
        coverage_0xec06f2a9(0x426cd03e94ca51439dda1ba8d4ee7c4883cb0a2d495c5cbc30ba905b59b6eedd); /* statement */ 
Require.that(
            takerAccount.owner == g_taker,
            FILE,
            "Order taker mismatch",
            orderInfo.orderHash
        );

        // verify markets
coverage_0xec06f2a9(0x539d7eb9b3e526109e28a4c76c1645d7fb45b19fd24ebcbeea2b00ba9ccf40ba); /* line */ 
        coverage_0xec06f2a9(0xbe1df4d2b47cefeb575c2e4da9a630b870ee3c548cf276404af4244baba952cb); /* statement */ 
Require.that(
            (
                orderInfo.order.baseMarket == outputMarketId &&
                orderInfo.order.quoteMarket == inputMarketId
            ) || (
                orderInfo.order.quoteMarket == outputMarketId &&
                orderInfo.order.baseMarket == inputMarketId
            ),
            FILE,
            "Market mismatch",
            orderInfo.orderHash
        );

        // verify inputWei is non-zero
coverage_0xec06f2a9(0xf9deac455c9ce03db3153b9e5e5e610851b8ee162f92996542298b0867e4b204); /* line */ 
        coverage_0xec06f2a9(0x4af655061be857bee21995f9e61436b38dc8bc6974f6bfe6e479de635a2f207c); /* statement */ 
Require.that(
            !inputWei.isZero(),
            FILE,
            "InputWei is zero",
            orderInfo.orderHash
        );

        // verify inputWei is positive if-and-only-if:
        // 1) inputMarket is the baseMarket and the order is a buy order
        // 2) inputMarket is the quoteMarket and the order is a sell order
coverage_0xec06f2a9(0xe27631d3d9649b22764f20619a23a9dcd71994048c77de8137c9f34d53e2e78b); /* line */ 
        coverage_0xec06f2a9(0x44f7727aa35c77bc9d68ace32ef823d63c1c7f5e4795b33eb473d7f6de26c997); /* statement */ 
Require.that(
            inputWei.sign ==
                ((orderInfo.order.baseMarket == inputMarketId) == isBuy(orderInfo.order)),
            FILE,
            "InputWei sign mismatch",
            orderInfo.orderHash
        );
    }

    /**
     * Verifies that the order is decreasing the size of the maker's position.
     */
    function verifyDecreaseOnly(
        Types.Par memory oldInputPar,
        Types.Par memory newInputPar,
        Types.AssetAmount memory assetAmount,
        Account.Info memory makerAccount,
        uint256 outputMarketId
    )
        private
        view
    {coverage_0xec06f2a9(0x9d614a26b53196a750f7df2a7314a84187aacd1e09e5a93032037fb4270e0fbe); /* function */ 

        // verify that the balance of inputMarketId is not increased
coverage_0xec06f2a9(0x5b3ba0eb57188ac2d9b46a62ca5332736a6ef650bb019aadb03c784e289c5e3d); /* line */ 
        coverage_0xec06f2a9(0xd8f9a2b431308b72861e17e3b7c5cd5adbd1d8eeafc2a4e0a38715f5834df576); /* statement */ 
Require.that(
            newInputPar.isZero()
            || (newInputPar.value <= oldInputPar.value && newInputPar.sign == oldInputPar.sign),
            FILE,
            "inputMarket not decreased"
        );

        // verify that the balance of outputMarketId is not increased
coverage_0xec06f2a9(0xbf0f35c9859c191af8f7a194df9696a75ca6f932e06290cef1817fff9386ae6c); /* line */ 
        coverage_0xec06f2a9(0x14efc9d09924d8afe7ccb653d4b57030d230890f7907040e795165a1d8887c97); /* statement */ 
Types.Wei memory oldOutputWei = SOLO_MARGIN.getAccountWei(makerAccount, outputMarketId);
coverage_0xec06f2a9(0x32fea07b66e2b7fa588ed6d3a5d4f91139d4dd45da0aac2e1dcb07bd759992d0); /* line */ 
        coverage_0xec06f2a9(0x900129ec589ffce9173da707bfed5cf9955d7ade8521141177c7567f6fe89454); /* statement */ 
Require.that(
            assetAmount.value == 0
            || (assetAmount.value <= oldOutputWei.value && assetAmount.sign != oldOutputWei.sign),
            FILE,
            "outputMarket not decreased"
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
    {coverage_0xec06f2a9(0x629df24774e88a7e2c9da3a9e7c1b98a252e18be5f2b7c0b97bf588bf940d05d); /* function */ 

coverage_0xec06f2a9(0x31ff3d9be6170d0aa37251aa93a25473bde7471c32ebb1862ae0de3cd5690414); /* line */ 
        coverage_0xec06f2a9(0x69fd11d96572403454c8581cd5e691f8e972aab71687ef2c4bed160d701283ec); /* statement */ 
uint256 fee = orderInfo.fill.price.getPartial(orderInfo.fill.fee, PRICE_BASE);
coverage_0xec06f2a9(0x4ae12f07871fa3c23736dfb2374e194271ae652070ee1da448f395a856196c75); /* line */ 
        coverage_0xec06f2a9(0xe5ca4d96cadb2d7a5c85c85987e463da4b0fa4c1b8ed190be25cf147cf0d3c50); /* statement */ 
uint256 adjustedPrice = (isBuy(orderInfo.order) == orderInfo.fill.isNegativeFee)
            ? orderInfo.fill.price.sub(fee)
            : orderInfo.fill.price.add(fee);

coverage_0xec06f2a9(0x9a0eddc46b16c49362bd64d8618d2d10db2bb2a111c0dee532a23aca49685b57); /* line */ 
        coverage_0xec06f2a9(0xd96cd61c95bb2aa4f1ae339502c06e8876a651495e56b725cd81f0f794490f34); /* statement */ 
uint256 outputAmount;
coverage_0xec06f2a9(0x011f781ccd9c058c09d041867e648fc500675d4c0077a1d5c90e8bc7587043af); /* line */ 
        coverage_0xec06f2a9(0xf35d4425ef62dac9ad8f996f928fe1742d485105cffb1ae86bf2972272344c79); /* statement */ 
uint256 fillAmount;
coverage_0xec06f2a9(0x4591c89060fe3cac2f8cd01c6a311cf5c810ddec139c9630cc85e3eefaeb0d6b); /* line */ 
        coverage_0xec06f2a9(0x0f8662293677884f1f7223db56faf6d41b250ad95917dda4415cccbd963a801b); /* statement */ 
if (orderInfo.order.quoteMarket == inputMarketId) {coverage_0xec06f2a9(0xaf6621fa5c285f48914512476a86af0fa643fbbfb4e7204414fd3004f80a9354); /* branch */ 

coverage_0xec06f2a9(0x84d08a82739b1efbf69f3547bc97472d61d112e3b5d22823dea3004381f57f37); /* line */ 
            coverage_0xec06f2a9(0x41c6b15cc83fca3867c8af4e1f85e73cbfb5787078647007fe06189a1070343f); /* statement */ 
outputAmount = inputWei.value.getPartial(PRICE_BASE, adjustedPrice);
coverage_0xec06f2a9(0x7865f74912066fd9743202f4ed56c5f17fbce4d1d93cc8228a0b38e7bc709dbc); /* line */ 
            coverage_0xec06f2a9(0x4ef905f525ae1512a81ddb3f5a329baa25f966d3a33623095b264da409ced2d8); /* statement */ 
fillAmount = outputAmount;
        } else {coverage_0xec06f2a9(0x46e4a566c7703b3682f250113fba990dfd1b4a9611fb0dcb888a480923c70460); /* branch */ 

coverage_0xec06f2a9(0xfa1ef60878b86676102458cbdeb46115217b654bb814c8cef6bf693d28b19c73); /* line */ 
            coverage_0xec06f2a9(0x67a8e6569545197a686fb162c03db58908999f47891082e1b82b2a71af45062e); /* assertPre */ 
coverage_0xec06f2a9(0x03098819093d3c22e937f4c4cd5fae0cf40a7a708562c028a9396fa2138a4743); /* statement */ 
assert(orderInfo.order.quoteMarket == outputMarketId);coverage_0xec06f2a9(0x4026e2551046673b56ad65d44bf84846a86f990ebb3f41b97c1b637c74b97848); /* assertPost */ 

coverage_0xec06f2a9(0x9c6937e0dbef8caf1695fa1317dba08f625426e7390146f628ff2d7acf4574d1); /* line */ 
            coverage_0xec06f2a9(0x13e14b689eba2948e67acb39c758210c1a971a8cbfa44a4c8f5169e1d894dd61); /* statement */ 
outputAmount = inputWei.value.getPartial(adjustedPrice, PRICE_BASE);
coverage_0xec06f2a9(0x98b60a7337063ffdd16c231c11b2c7b3ea6737ffbac98a6e52d781704b38a110); /* line */ 
            coverage_0xec06f2a9(0x32ee82cac6f1d985ccd8062ab5c51942679bdd0b317904d3fff65f654977253f); /* statement */ 
fillAmount = inputWei.value;
        }

coverage_0xec06f2a9(0x9b2fb68ebfadc37e16e436f53b5ceefe5f8e558e2c80df819da4d0cc10560e22); /* line */ 
        coverage_0xec06f2a9(0x567c7502ab4bf388fd79aef4fc9400b721582ae559847317e2a57afa973e30b0); /* statement */ 
updateFilledAmount(orderInfo, fillAmount);

coverage_0xec06f2a9(0xb0f3449d3ceb128ab29dfa4f2519980cd399078eef9656461a9674b15dcdef7b); /* line */ 
        coverage_0xec06f2a9(0xcc419aad18be72d152e6740fddadd7f4fa75a4d156946088a2ae633eb865eed9); /* statement */ 
return Types.AssetAmount({
            sign: !inputWei.sign,
            denomination: Types.AssetDenomination.Wei,
            ref: Types.AssetReference.Delta,
            value: outputAmount
        });
    }

    /**
     * Increases the stored filled amount of the order by fillAmount.
     * Returns the new total filled amount.
     */
    function updateFilledAmount(
        OrderInfo memory orderInfo,
        uint256 fillAmount
    )
        private
    {coverage_0xec06f2a9(0x40480507ab2bc4e8124a4e69f4e351c779fb363c935432db8ae58901308f5343); /* function */ 

coverage_0xec06f2a9(0xea3e5b9bc2f15484099d0ad7130ae7fe57fb3f28bc166d8147c350ac0e03c2c2); /* line */ 
        coverage_0xec06f2a9(0xb2a41f4c7a600c090a8b52d033b87b4d80a5cd631bd19995f58f02cb82ef5794); /* statement */ 
uint256 oldFilledAmount = g_filledAmount[orderInfo.orderHash];
coverage_0xec06f2a9(0x6d3868af9f0e2a505273a08711ad4a1cc92c00bd057170a56d1d86066c7d0bd0); /* line */ 
        coverage_0xec06f2a9(0x087cd2feef3ac95ad1004088c97dd4ac48d95f33f70afbd540d5bda78a2c8006); /* statement */ 
uint256 totalFilledAmount = oldFilledAmount.add(fillAmount);
coverage_0xec06f2a9(0xccd620d546880e95e5faa884a79aa63f4edcff21917f0713a1bbef3c3eb9f316); /* line */ 
        coverage_0xec06f2a9(0x0c7a7e63ecf041c87d00c662836d22032702c9e675923d246bd470d347be4697); /* statement */ 
Require.that(
            totalFilledAmount <= orderInfo.order.amount,
            FILE,
            "Cannot overfill order",
            orderInfo.orderHash,
            oldFilledAmount,
            fillAmount
        );

coverage_0xec06f2a9(0xcec848899258d735385039ab15b6b1088604bfec89c8823eebd6ed6318be8a17); /* line */ 
        coverage_0xec06f2a9(0x3fceb0ef7ab20cb24493fc71ad5a4edc1ebd4eab5dbc254b50ce43cfc508e7e4); /* statement */ 
g_filledAmount[orderInfo.orderHash] = totalFilledAmount;

coverage_0xec06f2a9(0xbb3a08f0d07f6242c0d5f948a33c529eacd64c1123e43b159b8595dc74445e17); /* line */ 
        coverage_0xec06f2a9(0x9d2fbf96e5bf95268c71643a5cdde366503320a7c5325ce75e2a8feb4b5d0bb4); /* statement */ 
emit LogCanonicalOrderFilled(
            orderInfo.orderHash,
            orderInfo.order.makerAccountOwner,
            fillAmount,
            orderInfo.order.triggerPrice,
            orderInfo.order.flags,
            orderInfo.fill
        );
    }

    /**
     * Returns the current price of baseMarket divided by the current price of quoteMarket. This
     * value is multiplied by 10^18.
     */
    function getCurrentPrice(
        uint256 baseMarket,
        uint256 quoteMarket
    )
        private
        view
        returns (uint256)
    {coverage_0xec06f2a9(0x9350e1fa4791384a5656df369f3165936e2615da7f0d2e355970ecca9452be30); /* function */ 

coverage_0xec06f2a9(0x4463eadca6362fb1596b3e2f6c9d8c9c78778c331e1dbb227f8ba8e816b00a3d); /* line */ 
        coverage_0xec06f2a9(0x057425af924ea25f950db431e10604a0ea01cd7a2d330d8f0de0bba8f136222e); /* statement */ 
Monetary.Price memory basePrice = SOLO_MARGIN.getMarketPrice(baseMarket);
coverage_0xec06f2a9(0x2b0ca34f612517bab728096e7f2646704c54d795e7b823f43698c3dbf3a2fab2); /* line */ 
        coverage_0xec06f2a9(0x915287c58792228a5e44f585d25bf8ed5a732de17c70a03c10efba86e0af9b03); /* statement */ 
Monetary.Price memory quotePrice = SOLO_MARGIN.getMarketPrice(quoteMarket);
coverage_0xec06f2a9(0x1bb234e14a36ce4c18198eb6ad6e23424f15633acd2f0129bd8706e94acdfd90); /* line */ 
        coverage_0xec06f2a9(0xa4acc57f2a61352dff383607eb9b8d4a2712f232b7894434c86626bdcb930a0b); /* statement */ 
return basePrice.value.mul(PRICE_BASE).div(quotePrice.value);
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
    {coverage_0xec06f2a9(0x64ea7e2886c9c67e7b30c551d07f674ec7ed50035b31fd7e92e6df07f54ffb1e); /* function */ 

        // compute the overall signed struct hash
        /* solium-disable-next-line indentation */
coverage_0xec06f2a9(0x0f1d2708ac873b0d50a938725666a0911fa2c1a086f96df3d7a748d7e4e56bb3); /* line */ 
        coverage_0xec06f2a9(0x1657a27d0b5a563e0c8df6582a78e7df9d9dffa463ead98f593d3885f75445ca); /* statement */ 
bytes32 structHash = keccak256(abi.encode(
            EIP712_ORDER_STRUCT_SCHEMA_HASH,
            order
        ));

        // compute eip712 compliant hash
        /* solium-disable-next-line indentation */
coverage_0xec06f2a9(0xa665e00a4c3d2153463db882ee76d2e40fe002f4abd1d8ed96d2fac7abe8c04e); /* line */ 
        coverage_0xec06f2a9(0x6c9669dc4e8b0da8e4943621b9ebbb5f733659deaaeccf619eb0ab5adcc2c6e3); /* statement */ 
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
    {coverage_0xec06f2a9(0x8fd7663bb01c50c62b28fc43d1910e88608c34caefb374a2b37ad15c1ac6380e); /* function */ 

coverage_0xec06f2a9(0xd21b7d9b4ce55431e1bb149202d3d6946ff611e9d81021af98c0f27866b62c0a); /* line */ 
        coverage_0xec06f2a9(0xa7bcfdcbb114107a5de84870d2c38e6f95f31ac48ad172772ac701eef2ebb9fa); /* statement */ 
Require.that(
            data.length == NUM_ORDER_AND_FILL_BYTES + NUM_SIGNATURE_BYTES,
            FILE,
            "Cannot parse signature from data"
        );

coverage_0xec06f2a9(0x2a27b25c64e3b91ce9729caa9aa1a0da422cdf9bbb97ee45a7aefc6dbbbf52cb); /* line */ 
        coverage_0xec06f2a9(0x4cdf6f927d362dab72e5889c26233042cfe23554e3047984c8f2d9a037be2dee); /* statement */ 
bytes memory signature = new bytes(NUM_SIGNATURE_BYTES);

coverage_0xec06f2a9(0xa57211abddcc52d35bfc54bb1ec9aa73625123f7e8696a7c73c58bcfad160fa2); /* line */ 
        coverage_0xec06f2a9(0xb51d4bf61450ce154842a3fb932c993762ebd7ee5ff41ac9511b7a060fab6337); /* statement */ 
uint256 sigOffset = NUM_ORDER_AND_FILL_BYTES;
        /* solium-disable-next-line security/no-inline-assembly */
coverage_0xec06f2a9(0x477d59828d5e347c4f9635546b28937dde8c6b7463cba78e30ee0a3f80964ab3); /* line */ 
        assembly {
            let sigStart := add(data, sigOffset)
            mstore(add(signature, 0x020), mload(add(sigStart, 0x20)))
            mstore(add(signature, 0x040), mload(add(sigStart, 0x40)))
            mstore(add(signature, 0x042), mload(add(sigStart, 0x42)))
        }

coverage_0xec06f2a9(0xce28f353b7f0ae22ac360bb4267f35f07de83beaf660b4e2ac5e69d4073282c5); /* line */ 
        coverage_0xec06f2a9(0x59b65848d25ca11309ba5dddb56cb122aeec81dce90d3d7a5b6f59adc4c28ca2); /* statement */ 
return signature;
    }

    /**
     * Returns true if the order is a buy order.
     */
    function isBuy(
        Order memory order
    )
        private
        pure
        returns (bool)
    {coverage_0xec06f2a9(0xcdab2a5e2639c3f761b35f8027a0c760332123ce016923dbf5b2cd24dc790c88); /* function */ 

coverage_0xec06f2a9(0xbfcca8e5408890f5ebc63501ac237b9687c3e457771f8b2f14de6a9ae7e400df); /* line */ 
        coverage_0xec06f2a9(0xae4b3f21ae6d837f3cb0b75cee0eb0b34141492c89a4a5b522d5dc2a7de93bcd); /* statement */ 
return (order.flags & IS_BUY_FLAG) != bytes32(0);
    }

    /**
     * Returns true if the order is a decrease-only order.
     */
    function isDecreaseOnly(
        Order memory order
    )
        private
        pure
        returns (bool)
    {coverage_0xec06f2a9(0x78542157beb9db7358b5d526117e3783b734031535fb126af25be2ea7f360da8); /* function */ 

coverage_0xec06f2a9(0x066e5f327b7b2095a9017fde312289094d01ce50142216d24bcf8414de69e855); /* line */ 
        coverage_0xec06f2a9(0xa37daf1cbae2278a315565955695c5a541ef935e9c2d8421f55b58df183ac392); /* statement */ 
return (order.flags & IS_DECREASE_ONLY_FLAG) != bytes32(0);
    }

    /**
     * Returns true if the order's limitFee is negative.
     */
    function isNegativeLimitFee(
        Order memory order
    )
        private
        pure
        returns (bool)
    {coverage_0xec06f2a9(0x02a4fb18a790a74decbcb5426e3c1f2cf5b8b431182da87793ecfa623fdee58b); /* function */ 

coverage_0xec06f2a9(0xac7a14001ef5712b9a7733bb30b3631f452b249762385d9268103a135a454f3d); /* line */ 
        coverage_0xec06f2a9(0xc9f444b7c357c4224bbd285c29fd142763bf1f2fc8966ad101d35b6398502466); /* statement */ 
return (order.flags & IS_NEGATIVE_FEE_FLAG) != bytes32(0);
    }
}
