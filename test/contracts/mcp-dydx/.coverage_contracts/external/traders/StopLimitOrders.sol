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
 * @title StopLimitOrders
 * @author dYdX
 *
 * Allows for Stop-Limit Orders to be used with dYdX
 */
contract StopLimitOrders is
    Ownable,
    OnlySolo,
    IAutoTrader,
    ICallee
{
function coverage_0x72322fd7(bytes32 c__0x72322fd7) public pure {}

    using Math for uint256;
    using SafeMath for uint256;
    using Types for Types.Par;
    using Types for Types.Wei;

    // ============ Constants ============

    bytes32 constant private FILE = "StopLimitOrders";

    // EIP191 header for EIP712 prefix
    bytes2 constant private EIP191_HEADER = 0x1901;

    // EIP712 Domain Name value
    string constant private EIP712_DOMAIN_NAME = "StopLimitOrders";

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

    // Hash of the EIP712 StopLimitOrder struct
    /* solium-disable-next-line indentation */
    bytes32 constant private EIP712_ORDER_STRUCT_SCHEMA_HASH = keccak256(abi.encodePacked(
        "StopLimitOrder(",
        "uint256 makerMarket,",
        "uint256 takerMarket,",
        "uint256 makerAmount,",
        "uint256 takerAmount,",
        "address makerAccountOwner,",
        "uint256 makerAccountNumber,",
        "address takerAccountOwner,",
        "uint256 takerAccountNumber,",
        "uint256 triggerPrice,",
        "bool decreaseOnly,",
        "uint256 expiration,",
        "uint256 salt",
        ")"
    ));

    // Number of bytes in an Order struct
    uint256 constant private NUM_ORDER_BYTES = 384;

    // Number of bytes in a typed signature
    uint256 constant private NUM_SIGNATURE_BYTES = 66;

    // Number of bytes in a CallFunctionData struct
    uint256 constant private NUM_CALLFUNCTIONDATA_BYTES = 32 + NUM_ORDER_BYTES;

    // The number of decimal places of precision in the price ratio of a triggerPrice
    uint256 PRICE_BASE = 10 ** 18;

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
        uint256 triggerPrice;
        bool decreaseOnly;
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

    event LogStopLimitOrderCanceled(
        bytes32 indexed orderHash,
        address indexed canceler,
        uint256 makerMarket,
        uint256 takerMarket
    );

    event LogStopLimitOrderApproved(
        bytes32 indexed orderHash,
        address indexed approver,
        uint256 makerMarket,
        uint256 takerMarket
    );

    event LogStopLimitOrderFilled(
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
    {coverage_0x72322fd7(0x0aebdc21b742522f46899932e1c103f471d2b7c6ba9cbcc69f195dd781240605); /* function */ 

coverage_0x72322fd7(0x6558729f0278410e6784dbc9c280e70fbb52d3cc3df54818b9166037caa8211d); /* line */ 
        coverage_0x72322fd7(0x8505b6fa85a1a8d26d402a3552a14fb6e2cb6759caf3e8422d53878254a5f031); /* statement */ 
g_isOperational = true;

        /* solium-disable-next-line indentation */
coverage_0x72322fd7(0xa89306eb3f49487c6c19075ee74cd9a16060b0ba1d168ca63d0b06844a034fc0); /* line */ 
        coverage_0x72322fd7(0x8eec3d44a5875af8ee113edd18bac916997c58be0c07b92c3b81c4043d9d1f11); /* statement */ 
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
    {coverage_0x72322fd7(0x53c821bc7ef9cbdc0b0ffd8bc648556e0d56486fc796e87f1c98696312c7edc8); /* function */ 

coverage_0x72322fd7(0x7d24300926ecef0d4b6676ee8421eefeb61d8e86bdf7f8e42b0ab395177b9895); /* line */ 
        coverage_0x72322fd7(0x32e3585e95fea0bc1cef3616475a7ac1f5a072ce6be5c4d0bd7ce212baf9594a); /* statement */ 
g_isOperational = false;
coverage_0x72322fd7(0xd1dd89407131947f73786355afa83e10c0f0a8df00d9122525bb8d35b2c30227); /* line */ 
        coverage_0x72322fd7(0x37c2b49d547a8355868690c57bdd115842e60b037d0ea661fc2131d0a791addb); /* statement */ 
emit ContractStatusSet(false);
    }

    /**
     * The owner can start back up the exchange.
     */
    function startUp()
        external
        onlyOwner
    {coverage_0x72322fd7(0x6962aea18a23b2436885477412f335d2103f6a2e03016cdcf1973187a9a041ca); /* function */ 

coverage_0x72322fd7(0xf83d2bb68a62e536410753d9d811adf4547aafe3999ef161c9c6c88f2c27eff8); /* line */ 
        coverage_0x72322fd7(0xc3dfc56ee8af1e2aa9dcd7eb8f5c88dda54a6b422041c500d020482b4e9d041a); /* statement */ 
g_isOperational = true;
coverage_0x72322fd7(0xb423896ad1c59c750833735c29de28699b982a756ec9df1defbe23f3b4997de1); /* line */ 
        coverage_0x72322fd7(0x44cbf0e7658f49d62f13d30c9529c3e73cebbdf25213023e781230a4679d4b65); /* statement */ 
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
    {coverage_0x72322fd7(0x8439a6cd5f9abbf25557a6695e627b925918e6f130d5169aec22d3a862f7c8f2); /* function */ 

coverage_0x72322fd7(0x45ef338e04d9ad2838d1961ce0d1f0deaa39341452c1db1348780bcf6bfeba6f); /* line */ 
        coverage_0x72322fd7(0x638a6501d44dc1c32f63eaf1ee3c0e808d5b1d31dd4a7f3594e7881dbb308a13); /* statement */ 
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
    {coverage_0x72322fd7(0x499b0867d1fb970c84e664da3b69183f4a7150fb5c7da77024e518bc0ebc9c8b); /* function */ 

coverage_0x72322fd7(0x890dbb99ac40e5147c584f1ece7fc2847b01de9ec7b62b467749cfc91ae2073d); /* line */ 
        coverage_0x72322fd7(0xad98e6f2cd62da729565d258977aa398118a07b2650ce938a8b5acfb4dcec795); /* statement */ 
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
    {coverage_0x72322fd7(0x0b3e733647fa716142152e95957f29628eca4eac09b867bf7113d15e1842026c); /* function */ 

coverage_0x72322fd7(0xf03e3fe0484a053b9ec39484af4c956d95c4440da851f7fb86805fd014b220f4); /* line */ 
        coverage_0x72322fd7(0xa5734df9678f41d9ab3ae6327e36b22d44e14b3b76336b976dfcac00750083a5); /* statement */ 
Require.that(
            g_isOperational,
            FILE,
            "Contract is not operational"
        );

coverage_0x72322fd7(0x6a4ad45aa51f381c369dbadcce9725c8286b080eca8d8fc60c1f286eaa104a3b); /* line */ 
        coverage_0x72322fd7(0xcaf9edcc968b220d308799790eb3640f1d0e01ff1cd01b85c1531b1bc20190ad); /* statement */ 
OrderInfo memory orderInfo = getOrderAndValidateSignature(data);

coverage_0x72322fd7(0x18a2ee87f318b43d6d3762815bf6322840cfe47af67fd83ae69988fc0a9cdc97); /* line */ 
        coverage_0x72322fd7(0xb19de0e69075764a2a2ac3a5c6177ea4948a08350d69f897289877189abca0b3); /* statement */ 
verifyOrderAndAccountsAndMarkets(
            orderInfo,
            makerAccount,
            takerAccount,
            inputMarketId,
            outputMarketId,
            inputWei
        );

coverage_0x72322fd7(0xece33cc178a4d5ac35c774e9e0a77a16e6656cc226a49b776f1ab63c29196ff3); /* line */ 
        coverage_0x72322fd7(0xe1430a37bfb6c8cf1ac8c3b750b2912b8e5e1c45db6a99181d7bf90156ff351c); /* statement */ 
Types.AssetAmount memory assetAmount = getOutputAssetAmount(
            inputMarketId,
            outputMarketId,
            inputWei,
            orderInfo
        );

coverage_0x72322fd7(0x6cdf265472f12353714fe7318840598f30bd81788bf91c18f59775b1f7ebf3bd); /* line */ 
        coverage_0x72322fd7(0x87148c1451a20eda230ea4a481de306649ec14bccfe3f4bfcb4c2f8f6fc1e8ab); /* statement */ 
if (orderInfo.order.decreaseOnly) {coverage_0x72322fd7(0x2fb60709675a0e86861aa1a54ae4c99a221727afc8a1cb879face36643e23ffd); /* branch */ 

coverage_0x72322fd7(0x5fe06af9ea04ad115a6d0e92d251d3a657faa236e26ced22059c6fe0aaeedd53); /* line */ 
            coverage_0x72322fd7(0xe91ebd0ac317f0e047ff18816d079f934bf4913577f7dd2fd2ae7932938fb1b2); /* statement */ 
verifyDecreaseOnly(
                oldInputPar,
                newInputPar,
                assetAmount,
                makerAccount,
                outputMarketId
            );
        }else { coverage_0x72322fd7(0x791b611ec6074616881ebd89786e767d0582d167667292e37d2a1da845184ed1); /* branch */ 
}

coverage_0x72322fd7(0xc12a15cbf6a38aff17d1b20b759e396a59ad703972ce8b0db726bc2417420635); /* line */ 
        coverage_0x72322fd7(0x0aa1f7624c8f6466a775487c2ffefbf1fd49e471cfb5df8fd0b8e24b2da3caa0); /* statement */ 
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
    {coverage_0x72322fd7(0x0c1b4fbb454163ddb4e66153ee963ef23a7664f0c6026c152bbb97bf3492699e); /* function */ 

coverage_0x72322fd7(0x8a5b09b723ddb35c8383591637b1069a6a48f91a7a32e7f5f0bc55d535f1e81c); /* line */ 
        coverage_0x72322fd7(0xc4346a66aba8b9f37c3f2fef55eb861490d0c03d69d9c6c5480e6226fa8b35b6); /* statement */ 
Require.that(
            data.length == NUM_CALLFUNCTIONDATA_BYTES,
            FILE,
            "Cannot parse CallFunctionData"
        );

coverage_0x72322fd7(0xb1f860db158001d296927efd3a9b15b6ae79178c6fa55fc239f1c048422f769c); /* line */ 
        coverage_0x72322fd7(0xc70ad98cde67d7a77655eee4017e89c922c47249ee98233f37cbb7f698dc6d2b); /* statement */ 
CallFunctionData memory cfd = abi.decode(data, (CallFunctionData));

coverage_0x72322fd7(0x0cde673fa913cccdcaca1fc207345edfab65e8a1b8ae8e9ce25feab46d92c1ad); /* line */ 
        coverage_0x72322fd7(0x4945805a28e23627fa4ceb45908bf4fa39891fe7e16341e65d2a82fee729b3d9); /* statement */ 
if (cfd.callType == CallFunctionType.Approve) {coverage_0x72322fd7(0x5bb9d8fecbd84402a6796fa47c5e7daa9404315988a76e9752e4dcc7029f99b2); /* branch */ 

coverage_0x72322fd7(0x0e4a7c92c911413f1f9c0cd85a66c94cda6b83c9e1bfa59d26ea318cc5db0088); /* line */ 
            coverage_0x72322fd7(0x3200b38b18206b09fdc13edf343e4961d18c354e245747b6c4f2917275cbe36a); /* statement */ 
approveOrderInternal(accountInfo.owner, cfd.order);
        } else {coverage_0x72322fd7(0x21a0331fce629ad5b2565a5474f726a1bf3de96a36b65c37b7f226bb260248fb); /* branch */ 

coverage_0x72322fd7(0x3c8ade35679d2fcb3f94519dc6a65465738cd2866949d0381a14ff9d019a7b0a); /* line */ 
            coverage_0x72322fd7(0x724b02c4b9c7c4fc0eb6138e087a62d0ffc3f1764c85b0f53bf4565e5e930048); /* assertPre */ 
coverage_0x72322fd7(0x24f2a4b21aa6736f3646020acacb74bdc625b0e4e6e34304c343588846164d0c); /* statement */ 
assert(cfd.callType == CallFunctionType.Cancel);coverage_0x72322fd7(0xbc04a2085e474861450f0bf7eb152d2475d910475012239f5e426ae2eb235a44); /* assertPost */ 

coverage_0x72322fd7(0x283f341d59d5308a996a392146d8d3443371f0634abc3f16c0f3692aa23fd77a); /* line */ 
            coverage_0x72322fd7(0xafcc50cf55a351cb2e957039cb2986920ec338a3021ccc4652b4998f366da100); /* statement */ 
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
    {coverage_0x72322fd7(0x7a48b05e5ff6d4c65d97be320413cc39da7820d9117ac412c1a2fc50b33182b3); /* function */ 

coverage_0x72322fd7(0xffda3deffd74dba81d709f1d11cbc42ee6b5709de9f3ade56409a0036edbda6d); /* line */ 
        coverage_0x72322fd7(0x702d9319f94972ddbec780fa5e7ea8a1f84500d4c3dc429b59a18a098f89c153); /* statement */ 
uint256 numOrders = orderHashes.length;
coverage_0x72322fd7(0xbe547aa4f6ecd4a46c6623476b319279b8b079b9f02f337015fc16bfa134e032); /* line */ 
        coverage_0x72322fd7(0x54f41075eba38fdae38070cb04c8dc3800e8e592c842389153b62f2842a72be8); /* statement */ 
OrderQueryOutput[] memory output = new OrderQueryOutput[](numOrders);

        // for each order
coverage_0x72322fd7(0xe4924e0631b1cd886bd1f5f37fd9302ff2a39bb66a82de9621a5ae2103a0e1b2); /* line */ 
        coverage_0x72322fd7(0x55419d7c81fe490aa5cb7ebf7b4834935d9a19c94b2cf870b840ed01a283c4e4); /* statement */ 
for (uint256 i = 0; i < numOrders; i++) {
coverage_0x72322fd7(0x88452e296b0fbc5f6a227cd67364258697c889e1a99ddfde80be8eac4ac5c63a); /* line */ 
            coverage_0x72322fd7(0x6dc1c46f04641418e4ba35ce9288170b43d61cec2d4ff7f02c62eb32cebecac3); /* statement */ 
bytes32 orderHash = orderHashes[i];
coverage_0x72322fd7(0x2564c7b920eaed18248ff2ff8476c811d5cb46a956757c059c3e5cfb0bd1ebb8); /* line */ 
            coverage_0x72322fd7(0xd6efa221ecc853bc659addab79dffdb74fed581568059e86526ab670a3b71dda); /* statement */ 
output[i] = OrderQueryOutput({
                orderStatus: g_status[orderHash],
                orderMakerFilledAmount: g_makerFilledAmount[orderHash]
            });
        }
coverage_0x72322fd7(0x3570cc613794336d03e82e9b10f7fa70a9e84d7874d289be4b6e1fb8ebb82317); /* line */ 
        coverage_0x72322fd7(0xcb2ffa0dc817bf82bff5247a5befa1b7c11144706e2dd501b321b61efb302f4a); /* statement */ 
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
    {coverage_0x72322fd7(0xe2a344ea47dda00a3b388238c03382ea2a4ae06d035b1e38beb5f9f784d0c2e0); /* function */ 

coverage_0x72322fd7(0x6888e8559989e366d8a010372ed97d459dcd95c16c5c6e514e599e89a361bb99); /* line */ 
        coverage_0x72322fd7(0x19e7da0b26a467c65f2e3f1a01a8b51f692626ae1f20b7d5fccde7a171749903); /* statement */ 
Require.that(
            canceler == order.makerAccountOwner,
            FILE,
            "Canceler must be maker"
        );
coverage_0x72322fd7(0x23537b142f5859e50402eaa2a4671beda8b11a691948b855ed2caddc03c6cc25); /* line */ 
        coverage_0x72322fd7(0x1371803d5169fd1a1e72e217452c0096dcc7943dd76c8a463c95174b50e8d560); /* statement */ 
bytes32 orderHash = getOrderHash(order);
coverage_0x72322fd7(0xebc474dc066a79a52868462d9956a302913f31019157fae098a1628289277203); /* line */ 
        coverage_0x72322fd7(0xf33e90329155f16f260bac2320131a8d8152ed5db84738dd2d695c10f5e4122e); /* statement */ 
g_status[orderHash] = OrderStatus.Canceled;
coverage_0x72322fd7(0x74963e71dc6ab0131a3a5820f6163a8c8bc2f11f8dd434399383dc98d36daff1); /* line */ 
        coverage_0x72322fd7(0x48a5a77a6e324ba78a5dbed60c1deec6e35aef964a1394491281e46702df50eb); /* statement */ 
emit LogStopLimitOrderCanceled(
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
    {coverage_0x72322fd7(0xe8f3177c50d0e465bd4af753d237f53df0378fbc32d80bc4212ecf4d09679193); /* function */ 

coverage_0x72322fd7(0xa25f699a41c49408d215e2260accf6f8e596753c1efb49b3958dc45af52380d9); /* line */ 
        coverage_0x72322fd7(0xe01a153828b9b752868c0f1e3c71d24ac05c9815cfd6abcdbb1ab2ef93425c7a); /* statement */ 
Require.that(
            approver == order.makerAccountOwner,
            FILE,
            "Approver must be maker"
        );
coverage_0x72322fd7(0xe2a1ef3b99a392216f6d90eaefef946fba4480e122afce5db43e7814ed6a8111); /* line */ 
        coverage_0x72322fd7(0xbbb5f3cbdf6b1abf07f2529846598eef44856203b54871539840990e149e595a); /* statement */ 
bytes32 orderHash = getOrderHash(order);
coverage_0x72322fd7(0x82bb4ae63d6ccfc2d5b2e7e21697fc4c7de4ac84a0ff44ca49bbe9b30721f33f); /* line */ 
        coverage_0x72322fd7(0x8c8b77acf950e54c8b159ff26d5481314643e1ff01f9f683bd21d065cf474335); /* statement */ 
Require.that(
            g_status[orderHash] != OrderStatus.Canceled,
            FILE,
            "Cannot approve canceled order",
            orderHash
        );
coverage_0x72322fd7(0xd3f88fdab6c1ac73cd65924253582fffbf65d5139ff7f6b7d6716ef4c9b24dad); /* line */ 
        coverage_0x72322fd7(0x246c965739b9494b15db590b23cfb49cd8cb796a05da12625ecc5c783328ab0c); /* statement */ 
g_status[orderHash] = OrderStatus.Approved;
coverage_0x72322fd7(0x7b25f179359d5de62084cfb95ef1fb2860b754e5f244a4b39768685f067184d0); /* line */ 
        coverage_0x72322fd7(0x833bd7c58296e8bd7f4805d30a2d67d0796cd2de70bc46391989c0ed20d3a6bf); /* statement */ 
emit LogStopLimitOrderApproved(
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
    {coverage_0x72322fd7(0xb005a69ad872a5fd8c8e464660a9c419318403bd9179ff82d773471f99af4f52); /* function */ 

        // verify triggerPrice
coverage_0x72322fd7(0x9c8b94dbfbb9606d69180e2a293d0be9de4fa5a9f01bf7d343c60ac907fb1d44); /* line */ 
        coverage_0x72322fd7(0xbdb274c240b66fab4c4818857b1509995778b35fd0e4f883dbeab62f1bc7fdc9); /* statement */ 
if (orderInfo.order.triggerPrice > 0) {coverage_0x72322fd7(0xa8e409791f21bff069d5d67aa066c47cb7c2553b214a1833ca805796122b3957); /* branch */ 

coverage_0x72322fd7(0x7f74286dddca2478e279d49db8cfce5c875fd1e2e05e160d7e0d5106a90c1f0e); /* line */ 
            coverage_0x72322fd7(0x46906962ef3a78909bce95a150fafd4bff037322e476b85c1a2681e4a63de15e); /* statement */ 
uint256 currentPrice = getCurrentPrice(
                orderInfo.order.makerMarket,
                orderInfo.order.takerMarket
            );
coverage_0x72322fd7(0x6f62031ffb5ec6476e90d54c76faa8b4a2a2e624f32b96a8cfa490f47512f691); /* line */ 
            coverage_0x72322fd7(0xf66bb36f8e6ee0e2be5781a87f26effdb47b6f298cd206c3dfe61abfea13e581); /* statement */ 
Require.that(
                currentPrice >= orderInfo.order.triggerPrice,
                FILE,
                "Order triggerPrice not triggered",
                currentPrice
            );
        }else { coverage_0x72322fd7(0x59daf671ef1cd8ccdfa6edc8073944da56def5d9f4f2d3aca4ae7988730bcfaf); /* branch */ 
}

        // verify expriy
coverage_0x72322fd7(0xb4a2d415dcb9870b95f296d8846627638eb56ef15da8ad1188ca5161a6759d6e); /* line */ 
        coverage_0x72322fd7(0x2977212af296d1d75ecdaaf8ca4275bd12d1646bd6e5de0a5f20ace480799ac2); /* statement */ 
Require.that(
            orderInfo.order.expiration == 0 || orderInfo.order.expiration >= block.timestamp,
            FILE,
            "Order expired",
            orderInfo.orderHash
        );

        // verify maker
coverage_0x72322fd7(0x5b662922fc25b3c60d928d461c7a3d286ce964fbca55d4ce8ded59472f6013e3); /* line */ 
        coverage_0x72322fd7(0x8daf388fe8a4bfcbc387696bdf2f04b8c6f19e1a0978773a26acd368a9b226c8); /* statement */ 
Require.that(
            makerAccount.owner == orderInfo.order.makerAccountOwner &&
            makerAccount.number == orderInfo.order.makerAccountNumber,
            FILE,
            "Order maker account mismatch",
            orderInfo.orderHash
        );

        // verify taker
coverage_0x72322fd7(0x9f1de703e820dc2968ea5f041efcc5b5a6d7c9a3daa421b7ddf3f816f59e5d7d); /* line */ 
        coverage_0x72322fd7(0x126dfcd946f568055186031880c213fa2c8b14be1d68cfb566768549b8031e92); /* statement */ 
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
coverage_0x72322fd7(0x578e36e530e54531925f2d8f2ebf280e98c771960da13f84e7a0623c7299f207); /* line */ 
        coverage_0x72322fd7(0x15bbe81f6141f0e7268a4726f23259cc2345d32a0bb35650b91d885d41777bba); /* statement */ 
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
coverage_0x72322fd7(0x5a4177750628e86f6948980b8595a44a65395c9f99b1f44ae01e1ff47addb1c3); /* line */ 
        coverage_0x72322fd7(0x3206cf343f931aeb98c4b46e0c048c11986b65ae3d81efc138e6497f6eed7d22); /* statement */ 
Require.that(
            !inputWei.isZero(),
            FILE,
            "InputWei is zero",
            orderInfo.orderHash
        );
coverage_0x72322fd7(0xdf3d1cd2e36c0732ac68b34fd19faae7ec8ea1bd6b0e5b762bec4a2c67fc00df); /* line */ 
        coverage_0x72322fd7(0xe0dc01d7c46662fbf0437c235960c7605bc87fed3fd36bcffddd4219b5dbc2bb); /* statement */ 
Require.that(
            inputWei.sign == (orderInfo.order.takerMarket == inputMarketId),
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
    {coverage_0x72322fd7(0x53fb647c13493ec44fb96933d34a0efd308d3ad4472480ffa76c12575a7f1a87); /* function */ 

        // verify that the balance of inputMarketId is not increased
coverage_0x72322fd7(0x47c7cf6fdb5e21bd3bf61bbe769bf6e2a2f1412dadd3766a3aae4a6f0138537d); /* line */ 
        coverage_0x72322fd7(0xcad4f566a3dcfaf8d44c616f7ae19cc79aa8707baa0ba87eb848b386392991c9); /* statement */ 
Require.that(
            newInputPar.isZero()
            || (newInputPar.value <= oldInputPar.value && newInputPar.sign == oldInputPar.sign),
            FILE,
            "inputMarket not decreased"
        );

        // verify that the balance of outputMarketId is not increased
coverage_0x72322fd7(0xd5ef315f7780cfb87402be10e0d012ebd3da2f34a07e109253572d2d3990ee2c); /* line */ 
        coverage_0x72322fd7(0x9244c7a0323f2b463e08a484f75e81e3a75e87ba7b72fa9d11f5ef10bea44bc2); /* statement */ 
Types.Wei memory oldOutputWei = SOLO_MARGIN.getAccountWei(makerAccount, outputMarketId);
coverage_0x72322fd7(0x6682ea12b4f6704c81e9fbb890818530365e8ed7a99daaeccdef14139ced3bc6); /* line */ 
        coverage_0x72322fd7(0x38a71ee803b840c273d3fd782cd78b5644b018ac626cea6379454a46fb1fd24d); /* statement */ 
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
    {coverage_0x72322fd7(0xb0da95e4009eeced9266a95439c6bad9bb8cd12c8a7049dc5d2ec0445d986f44); /* function */ 

coverage_0x72322fd7(0x985462720f49b5336356d3377d08ade1db52f3226d34ab5d15691d678f8046f3); /* line */ 
        coverage_0x72322fd7(0x3ec19023577ba4ce8993239177caa30bd92b5bfd851d103776eb2e981eadaaa2); /* statement */ 
uint256 outputAmount;
coverage_0x72322fd7(0x6348fa9b74fa201f0e218b63915d6f05690a3277aeb90ee9d36dd2f571abcd2c); /* line */ 
        coverage_0x72322fd7(0x0af3b62c23d909c33ab8428fe6067f3dc2abdeddf5cdbaef7a1dc897e9f4d0a3); /* statement */ 
uint256 makerFillAmount;

coverage_0x72322fd7(0x999597f698048683c1ed5685f73b419b455237e850e20411d77e2f5faccae107); /* line */ 
        coverage_0x72322fd7(0x36ee9ffbfaf756784c093c65f8b36f811b7bb35de77619cdf34b1b610dae9fd1); /* statement */ 
if (orderInfo.order.takerMarket == inputMarketId) {coverage_0x72322fd7(0x88693b1ed56ad0fadd0bb624121a35e8d48049c2035944eccf2ca940b47d0fd6); /* branch */ 

coverage_0x72322fd7(0xe1fbee45fcd6e5cef2e4756d67d943945bcb27dedbc286c8e3bf064d6700bd81); /* line */ 
            coverage_0x72322fd7(0x97a14e628432d5900a79b7559cc7dac1deeab2aa705dcc25cb3d8e3ca1137cee); /* statement */ 
outputAmount = inputWei.value.getPartial(
                orderInfo.order.makerAmount,
                orderInfo.order.takerAmount
            );
coverage_0x72322fd7(0x6c45addaf7ce220b21ab30f65574d8d2ff89452c0d16f0df80ab418cdb7722cd); /* line */ 
            coverage_0x72322fd7(0x81cd4ef77b9c3ee85b62c2f54afa44c2c5d79ae4322122223f6284881555b3c6); /* statement */ 
makerFillAmount = outputAmount;
        } else {coverage_0x72322fd7(0x7991683d409dbf3658e2651b48fbf22cfe5f7af8c1121a4e6e499f7927c3c17d); /* branch */ 

coverage_0x72322fd7(0xc1f6365c417706ccdf246e5fbe0e50e46bca2e7b08e79602a4f4936a2e3a94ed); /* line */ 
            coverage_0x72322fd7(0xdab5d34d1ff2bc165764913f2e80976e1d1490f40187891fbe572bbcb162e935); /* assertPre */ 
coverage_0x72322fd7(0x5cb702433e77a0b4a8faca2f4ce576300b6362ef388c22b4508d41cefe7d1de8); /* statement */ 
assert(orderInfo.order.takerMarket == outputMarketId);coverage_0x72322fd7(0xbfb7d8bad9bede7e33ede462304040bad61e028952f5df387880e83de2edd6a9); /* assertPost */ 

coverage_0x72322fd7(0xab899ca20206daa653539f7e720c6452a6c7368960c1594882ea39441436ffa5); /* line */ 
            coverage_0x72322fd7(0xbbbd4ebb04371ffc0cf339fffa690befbcfb984470e88ce538620301652307a3); /* statement */ 
outputAmount = inputWei.value.getPartialRoundUp(
                orderInfo.order.takerAmount,
                orderInfo.order.makerAmount
            );
coverage_0x72322fd7(0xdfc0c09db5477b0c7d10d41f957537de6c3d1581bfa45bad3e756f4a66ffaa00); /* line */ 
            coverage_0x72322fd7(0x5eac5f9b16134b8285934d8ce3692878efab11025bc666d27a2d87d97ea93d72); /* statement */ 
makerFillAmount = inputWei.value;
        }

coverage_0x72322fd7(0x18873df38e8cec8e4ebb7484612dbf4c169256f184a41bc1c379aae35088d53f); /* line */ 
        coverage_0x72322fd7(0x01848f1084d4d4aa49753866dccc30deef4ad5408bc8fe4b740c6bbdaf6844a7); /* statement */ 
updateMakerFilledAmount(orderInfo, makerFillAmount);

coverage_0x72322fd7(0x8d0e21ce3851c7cbaf8b91659c6dfdd26365c46da6ff03f1630bc2fce7301cef); /* line */ 
        coverage_0x72322fd7(0x22eb4025822e66b059ea9926cc2a148f10aafd8028a8f19e57fe264b50e997a2); /* statement */ 
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
    {coverage_0x72322fd7(0x6684cbf23505af27848cb3e9107e028094e6fd4e16c757a6133788e50e704b49); /* function */ 

coverage_0x72322fd7(0xc501a9581998d23eaf51b3a4901f61ac05425b16724981487e248931243286e2); /* line */ 
        coverage_0x72322fd7(0xf9278d707b52a4aa8a7690a826db36bd15f1d2a187a8859d6b301aca4747524c); /* statement */ 
uint256 oldMakerFilledAmount = g_makerFilledAmount[orderInfo.orderHash];
coverage_0x72322fd7(0x0fd1a1eca133890fc4f56b2f88c960d4b3fcc06f09cb9ef37df05f8dfbdbf2f9); /* line */ 
        coverage_0x72322fd7(0x9e2ad261879e233d717fc0a81306475f448e13cfdeb811433f1e5cce170b42c4); /* statement */ 
uint256 totalMakerFilledAmount = oldMakerFilledAmount.add(makerFillAmount);
coverage_0x72322fd7(0x98c9ba54798038fa1c5b8d05ca743f3628c28e125bc90f1f8c706a4615fec656); /* line */ 
        coverage_0x72322fd7(0x633d8ed69b494ec678ceff93ac9fbd807820458b2071edb54011cf7354b3d529); /* statement */ 
Require.that(
            totalMakerFilledAmount <= orderInfo.order.makerAmount,
            FILE,
            "Cannot overfill order",
            orderInfo.orderHash,
            oldMakerFilledAmount,
            makerFillAmount
        );

coverage_0x72322fd7(0x73fdbf497a12990b1389fa4b857ee2e2e271fcae4ab9a3b52a8712283a4ee86e); /* line */ 
        coverage_0x72322fd7(0xe8a0ec2a5d8acd5129e649afbba819b82edb66e0bd4d3c62904112a613184480); /* statement */ 
g_makerFilledAmount[orderInfo.orderHash] = totalMakerFilledAmount;

coverage_0x72322fd7(0x592e0625c7f3b1eeacfdab023664aca16df6aaa0543cdbdcd6c6e64655ddccce); /* line */ 
        coverage_0x72322fd7(0xfdd35a0104004a07d00b36edc93f54f23af742edce2f5c736ae72c13672623a3); /* statement */ 
emit LogStopLimitOrderFilled(
            orderInfo.orderHash,
            orderInfo.order.makerAccountOwner,
            makerFillAmount,
            totalMakerFilledAmount
        );
    }

    /**
     * Returns the current price of makerMarket divided by the current price of takerMarket. This
     * value is multiplied by 10^18.
     */
    function getCurrentPrice(
        uint256 makerMarket,
        uint256 takerMarket
    )
        private
        view
        returns (uint256)
    {coverage_0x72322fd7(0x77350e8586fe64c68c55146c58fda29b4e14d3a2d08768ec1691c777a43c4164); /* function */ 

coverage_0x72322fd7(0x03a972be8148ad3363f2271058f9ea1d9976ef30074d48eef34075c11178de64); /* line */ 
        coverage_0x72322fd7(0x72c55bf13f7b0c800b00fa5348bac1c4f142a39c73e5d2ea3c560df9917ed8d6); /* statement */ 
Monetary.Price memory takerPrice = SOLO_MARGIN.getMarketPrice(takerMarket);
coverage_0x72322fd7(0x61641abe477616678819c966aa66bf80498aa8ed166d8d82a5c389c2248a4644); /* line */ 
        coverage_0x72322fd7(0x75ffcf7f693ae3fa9fbfec701c4f0bee176badf3cbea1d430e2dde1a2501b211); /* statement */ 
Monetary.Price memory makerPrice = SOLO_MARGIN.getMarketPrice(makerMarket);
coverage_0x72322fd7(0x423ec8cfb54b00edcdf92daf0705960b199a468eb7e9f50122c1796dea5104e2); /* line */ 
        coverage_0x72322fd7(0xb87b8b291ff3105a08a46bdb064e84243f752f028047c71150004cd3cbbc445a); /* statement */ 
return takerPrice.value.mul(PRICE_BASE).div(makerPrice.value);
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
    {coverage_0x72322fd7(0xda0d2610b8644b5ff93ebb82c3d15088499d59b114ecb3646d929c47185dcaf7); /* function */ 

coverage_0x72322fd7(0xecd2612f38ca2db115bef1f68b2532b2b710ab11de5aee5d4123c5f42ed6cf81); /* line */ 
        coverage_0x72322fd7(0xd07571cc8bcbffd1b1e8ca62369de4af48c5d0387731dea43be341c2b274b93c); /* statement */ 
Require.that(
            (
                data.length == NUM_ORDER_BYTES ||
                data.length == NUM_ORDER_BYTES + NUM_SIGNATURE_BYTES
            ),
            FILE,
            "Cannot parse order from data"
        );

coverage_0x72322fd7(0xdb4ed0706bf96cf10f04b2a8aa466f44efc51a392b358d1525822f70ecb79e38); /* line */ 
        coverage_0x72322fd7(0x82768c0bb192e938fbb13faf786d99b7a9cf4835c72fcf797fa072c275f0c4db); /* statement */ 
OrderInfo memory orderInfo;
coverage_0x72322fd7(0x1710268d9be592cfef00beb27348431c48a17f27762b066fefce34f7c452d407); /* line */ 
        coverage_0x72322fd7(0xaf9155aad0060efb1db3b155c52df3e56ba130ac8d44a30f4f18c38159925493); /* statement */ 
orderInfo.order = abi.decode(data, (Order));
coverage_0x72322fd7(0x23460007a07b7d80080e26bbe227d54a87ec85558eace5708e51c0ce4d3cfab7); /* line */ 
        coverage_0x72322fd7(0xecb7c39b6153b7da9b7b4d69ce9b6d8483f62716bb533597194e4c4b2896ba77); /* statement */ 
orderInfo.orderHash = getOrderHash(orderInfo.order);

coverage_0x72322fd7(0xeff8db003cd66be76b7b08a4bcf5fffe82847671ef8e0f96614aee924d8d8c7d); /* line */ 
        coverage_0x72322fd7(0x925274af2aab82ef9fa3368aa063ca92e3023e2ad49c39daf45fffd6468b91c7); /* statement */ 
OrderStatus orderStatus = g_status[orderInfo.orderHash];

        // verify valid signature or is pre-approved
coverage_0x72322fd7(0x2a047be84017d78900817dbcc304178c0a2bf3e559a8f6ea41e4d06456ab9fd4); /* line */ 
        coverage_0x72322fd7(0x8c9a9b32e40ffab9cfe21502569b763a868c98d3021d0d4eb14ccac1c0ce3f2a); /* statement */ 
if (orderStatus == OrderStatus.Null) {coverage_0x72322fd7(0xe84e652e669fe4a541a9dab102d9fcea64f40cc0aecce15210c05443432160c7); /* branch */ 

coverage_0x72322fd7(0x68e099eb2bac0ad3070333d5fcc268416cbc54146f255493ec4b2c57dd0156b4); /* line */ 
            coverage_0x72322fd7(0x28211680e6b7dbb9daac5822b3e33bf2e85508201a3f37b2fc29fc03a24c1eb5); /* statement */ 
bytes memory signature = parseSignature(data);
coverage_0x72322fd7(0x94078df82ced922a5a94317648ef31ec6090c0db5d299bd2886b6404cef62030); /* line */ 
            coverage_0x72322fd7(0x0d30e95645d63bba9b26290465c346e25b9582130d4589890e38d99a55549a4e); /* statement */ 
address signer = TypedSignature.recover(orderInfo.orderHash, signature);
coverage_0x72322fd7(0x3ab70c478e667b4586f1fcc16d28d4fff4565f02043fe6048d9dd4404aa88d54); /* line */ 
            coverage_0x72322fd7(0x240292c3efc8253d15e4fb1a180ed397d530e699cfa5209a22d704ca9cff6359); /* statement */ 
Require.that(
                orderInfo.order.makerAccountOwner == signer,
                FILE,
                "Order invalid signature",
                orderInfo.orderHash
            );
        } else {coverage_0x72322fd7(0x0e7ec89db3668f2bb360496e8e9ed5c6841feb3aa8d03d1e773b715f4e08a48e); /* branch */ 

coverage_0x72322fd7(0xc90dbc415a1f6dfd476802742a350e2526553c4a1d1df1922d8f88024403c3a6); /* line */ 
            coverage_0x72322fd7(0x459c900fececd173c694ea20775d0c3aa2520c21d50bf767b2aa81e0ecb41ac7); /* statement */ 
Require.that(
                orderStatus != OrderStatus.Canceled,
                FILE,
                "Order canceled",
                orderInfo.orderHash
            );
coverage_0x72322fd7(0xfce84a47f3f5fd9d528934d8fb6133eb8226059ee2b0be07b015762dfad9032a); /* line */ 
            coverage_0x72322fd7(0x35b7acc4bc8724f6cb75ea394fb6b0b0f643ed87f71a45e0a77b538a96b2dc22); /* assertPre */ 
coverage_0x72322fd7(0x8860a4a5ed809936b99ea8087449dce52828bc3728b8ff1959c6aafea1e4b0c5); /* statement */ 
assert(orderStatus == OrderStatus.Approved);coverage_0x72322fd7(0xb67f5bbc8752d1decff00c83abc9c67b36e41c07bf8732fa948be3daf21c757f); /* assertPost */ 

        }

coverage_0x72322fd7(0x97027828a7722e704ab3e406b2ad7af561d440b1a6936814c0e095842037f435); /* line */ 
        coverage_0x72322fd7(0xe1e1938b8e3e45fc7f0bd89f459859f649dcc4b74b8c8099da307f763cccf6b7); /* statement */ 
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
    {coverage_0x72322fd7(0x22977de0b56dfe2c9b01f72580fca4ca72b481a64626b0595a166f6512e91a4a); /* function */ 

        // compute the overall signed struct hash
        /* solium-disable-next-line indentation */
coverage_0x72322fd7(0x2f701b503a52b5ded8beba55257f2028e2079d2068f19b48f5ca91b0e87c2b9f); /* line */ 
        coverage_0x72322fd7(0xccaba1351318643eff611c2429b43672f6a30624152b66b14aa75c37b81c6cd6); /* statement */ 
bytes32 structHash = keccak256(abi.encode(
            EIP712_ORDER_STRUCT_SCHEMA_HASH,
            order
        ));

        // compute eip712 compliant hash
        /* solium-disable-next-line indentation */
coverage_0x72322fd7(0x029e4eb54baa0c932e9c8146e514d4a690e5dd2eecfd8492e0f4d3cf2db1f0d5); /* line */ 
        coverage_0x72322fd7(0x6fe41c20cc167a53cceb85e72605e764c7c6f440691187072f44e6c8449a41ae); /* statement */ 
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
    {coverage_0x72322fd7(0xe93742190a554ceca949df5ee7c35806cd8f5be6661148266b0015b0ead439cb); /* function */ 

coverage_0x72322fd7(0xc5d69b561a12dda9f64edd69d3eb65a99eeeac9a72b6ad9276bfa0940ba71ff6); /* line */ 
        coverage_0x72322fd7(0xe3fdc639050a6fcf55719e49a6cd710e6b742737c90769e2abffe2849a2ed78f); /* statement */ 
Require.that(
            data.length == NUM_ORDER_BYTES + NUM_SIGNATURE_BYTES,
            FILE,
            "Cannot parse signature from data"
        );

coverage_0x72322fd7(0x2545262b7c602c3806bbabe90e163c038ace79c956a61ef068d59b5467d0b47d); /* line */ 
        coverage_0x72322fd7(0xa91c326e77c5cd5c94fe76346371cce0020b3bcb17c6611f3d5349637dd76e5f); /* statement */ 
bytes memory signature = new bytes(NUM_SIGNATURE_BYTES);

coverage_0x72322fd7(0x8998f33d11c5f7515720d836b8dfdc18637c55ff16160c5fcee84309bbba4740); /* line */ 
        coverage_0x72322fd7(0x9799f9736f2851b269ceaa317a25e9b02b0500ad9bc0d0b7c39793b7cba5ccef); /* statement */ 
uint256 sigOffset = NUM_ORDER_BYTES;
        /* solium-disable-next-line security/no-inline-assembly */
coverage_0x72322fd7(0xf64b9e765857c6965784218079110ec9cb160803a05e45b76112d9dfda7fd924); /* line */ 
        assembly {
            let sigStart := add(data, sigOffset)
            mstore(add(signature, 0x020), mload(add(sigStart, 0x20)))
            mstore(add(signature, 0x040), mload(add(sigStart, 0x40)))
            mstore(add(signature, 0x042), mload(add(sigStart, 0x42)))
        }

coverage_0x72322fd7(0x4e14f9ead108ce68242f146c6ebc5cf459f3001dbc99932a8a87695d98dcfeab); /* line */ 
        coverage_0x72322fd7(0xb77131bfc622852f16d87a7dc98ddc95bc85e90988db0d2547c54f4866bdcbd8); /* statement */ 
return signature;
    }
}
