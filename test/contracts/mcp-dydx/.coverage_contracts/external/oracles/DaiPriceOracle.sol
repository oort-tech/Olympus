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
import { IErc20 } from "../../protocol/interfaces/IErc20.sol";
import { IPriceOracle } from "../../protocol/interfaces/IPriceOracle.sol";
import { Math } from "../../protocol/lib/Math.sol";
import { Monetary } from "../../protocol/lib/Monetary.sol";
import { Require } from "../../protocol/lib/Require.sol";
import { Time } from "../../protocol/lib/Time.sol";
import { ICurve } from "../interfaces/ICurve.sol";
import { IUniswapV2Pair } from "../interfaces/IUniswapV2Pair.sol";


/**
 * @title DaiPriceOracle
 * @author dYdX
 *
 * PriceOracle that gives the price of Dai in USDC.
 */
contract DaiPriceOracle is
    Ownable,
    IPriceOracle
{
function coverage_0x7dac2259(bytes32 c__0x7dac2259) public pure {}

    using SafeMath for uint256;

    // ============ Constants ============

    bytes32 constant FILE = "DaiPriceOracle";

    // DAI decimals and expected price.
    uint256 constant DECIMALS = 18;
    uint256 constant EXPECTED_PRICE = ONE_DOLLAR / (10 ** DECIMALS);

    // Parameters used when getting the DAI-USDC price from Curve.
    int128 constant CURVE_DAI_ID = 0;
    int128 constant CURVE_USDC_ID = 1;
    uint256 constant CURVE_FEE_DENOMINATOR = 10 ** 10;
    uint256 constant CURVE_DECIMALS_BASE = 10 ** 30;

    // Parameters used when getting the DAI-USDC price from Uniswap.
    uint256 constant UNISWAP_DECIMALS_BASE = 10 ** 30;

    // ============ Structs ============

    struct PriceInfo {
        uint128 price;
        uint32 lastUpdate;
    }

    struct DeviationParams {
        uint64 denominator;
        uint64 maximumPerSecond;
        uint64 maximumAbsolute;
    }

    // ============ Events ============

    event PriceSet(
        PriceInfo newPriceInfo
    );

    // ============ Storage ============

    PriceInfo public g_priceInfo;

    address public g_poker;

    DeviationParams public DEVIATION_PARAMS;

    IErc20 public WETH;

    IErc20 public DAI;

    ICurve public CURVE;

    IUniswapV2Pair public UNISWAP_DAI_ETH;

    IUniswapV2Pair public UNISWAP_USDC_ETH;

    // ============ Constructor =============

    constructor(
        address poker,
        address weth,
        address dai,
        address curve,
        address uniswapDaiEth,
        address uniswapUsdcEth,
        DeviationParams memory deviationParams
    )
        public
    {coverage_0x7dac2259(0xd1d9180f86f90cd069389130a6c5c7d0e6ae767b5cb4c87d3455ed240726b236); /* function */ 

coverage_0x7dac2259(0xb84083d4bda61d0069b8f763ae8d0e10a1587b55ccd7e50517b23c41890cedcb); /* line */ 
        coverage_0x7dac2259(0xc2b357ba277e2fdf0a5a72f7259c41f04ef919aae95144873e83f029cdaccdca); /* statement */ 
g_poker = poker;
coverage_0x7dac2259(0xee8ab3d65494728c984af6d6344bd3d628c96f681072bae7495d30aae7393dfb); /* line */ 
        coverage_0x7dac2259(0x04ff0af6c0dd8a7d8b7330d9511e8c464a38ad32e2d6950e7a04bba41d1fbefc); /* statement */ 
WETH = IErc20(weth);
coverage_0x7dac2259(0x4d632463bec0c530e4a7d340b31a24f67789865b10864b69c60a2a9366f41137); /* line */ 
        coverage_0x7dac2259(0x3c0deb4c986fab9432cc69ccdbfb6fe532bf2c6e891c9b084dd93ce5041a841d); /* statement */ 
DAI = IErc20(dai);
coverage_0x7dac2259(0x0e02c0db3e163d1388a08d38c35a819e2ba09098e09a2bab4d0ebeedcd425bf3); /* line */ 
        coverage_0x7dac2259(0x597c326fd5fdeb84bd07c59c10d5b8880ea2dff9dbe927b930fdbee36e9e0209); /* statement */ 
CURVE = ICurve(curve);
coverage_0x7dac2259(0x9fc6b59911f050c00c270295a3c458156ca9a6b2b3b42060f8a14210566534b2); /* line */ 
        coverage_0x7dac2259(0x23d43bc7ac9825e77f1cf15ffedbc6bde691b2ec43943dc6a1bdfd2481b13b64); /* statement */ 
UNISWAP_DAI_ETH = IUniswapV2Pair(uniswapDaiEth);
coverage_0x7dac2259(0x737d84ee1b105e51560fc3f44422fce0798275fd4af3e1647d6de80945d18f6d); /* line */ 
        coverage_0x7dac2259(0x36a7e73ca04744654eae8715875746bec09ad7720565ebd1ac08259072cfd86c); /* statement */ 
UNISWAP_USDC_ETH = IUniswapV2Pair(uniswapUsdcEth);
coverage_0x7dac2259(0x8dddc85a7ea68cbec827d8f84ce549f701e94d60bf2bc969e8fc4140bda60753); /* line */ 
        coverage_0x7dac2259(0x871f646e5f27d1aba10215c0293927e02ec2544118365785ffe05c3d6b58a49b); /* statement */ 
DEVIATION_PARAMS = deviationParams;
coverage_0x7dac2259(0x3a044a27b60c6923e6e63583b0e419d805abeddd8ac6046824f0efdac1fccce3); /* line */ 
        coverage_0x7dac2259(0xde4d18c5d687ab8c46d7044c98c3948488b4830cc04b266845435c152d0d0315); /* statement */ 
g_priceInfo = PriceInfo({
            lastUpdate: uint32(block.timestamp),
            price: uint128(EXPECTED_PRICE)
        });
    }

    // ============ Admin Functions ============

    function ownerSetPokerAddress(
        address newPoker
    )
        external
        onlyOwner
    {coverage_0x7dac2259(0x78cd2661aa04e68323fc5c1ea2ea9f7bb6a56efcfd132f8424d87af5dbef50f3); /* function */ 

coverage_0x7dac2259(0x320307e0568b6d8b052f067176f9323359d3872bebdecdabc8ed144e9eb48f64); /* line */ 
        coverage_0x7dac2259(0x0fa7077bb034681938f197d0c7a3f81a4e7f193100efb0a50d3a30b689038b95); /* statement */ 
g_poker = newPoker;
    }

    // ============ Public Functions ============

    function updatePrice(
        Monetary.Price memory minimum,
        Monetary.Price memory maximum
    )
        public
        returns (PriceInfo memory)
    {coverage_0x7dac2259(0x25e55561ed33600752bb7b5aeeda892f8e9b2472982d4d544c3d4a704d4127e0); /* function */ 

coverage_0x7dac2259(0x44f39b7c9d62c85679079593c9007703acc1efe53ac051d314b7d1393f9b6e0c); /* line */ 
        coverage_0x7dac2259(0x43b7114b93bde0100c1f672195935d32d372d8d6cd8443e25c059447b6752594); /* statement */ 
Require.that(
            msg.sender == g_poker,
            FILE,
            "Only poker can call updatePrice",
            msg.sender
        );

coverage_0x7dac2259(0x990ea4c4cdde896c2215e8a075a0b860d10cedf8788e3119c24f99fcce86365b); /* line */ 
        coverage_0x7dac2259(0xf9eb5151b3b1d71a29d6a487ff586c0fdcb99ccb3945090bbb225c691d11bcca); /* statement */ 
Monetary.Price memory newPrice = getBoundedTargetPrice();

coverage_0x7dac2259(0x8f8c22923b6eedb05bf619de4b4a30ccec9f1a1c5e0adc07c8aa72ef0b7fbba2); /* line */ 
        coverage_0x7dac2259(0x8adb334ee24dfdaa84eeff926f5189bfdce38940fe5e36740e4594c8b77027a6); /* statement */ 
Require.that(
            newPrice.value >= minimum.value,
            FILE,
            "newPrice below minimum",
            newPrice.value,
            minimum.value
        );

coverage_0x7dac2259(0xf9b3987d715022881cc68fdb1a2835678529900536f1ebe292925ea9e2527d79); /* line */ 
        coverage_0x7dac2259(0x274c730f0cceac57976c2e04ec2720ba6c7fdd9ea16131c42d8614c2ace11dc4); /* statement */ 
Require.that(
            newPrice.value <= maximum.value,
            FILE,
            "newPrice above maximum",
            newPrice.value,
            maximum.value
        );

coverage_0x7dac2259(0x7868b325edd809c0a30eeb34ad1dab2d18971b7d3192fb891376f8613e1f5278); /* line */ 
        coverage_0x7dac2259(0xac64585149a988696ad50b0a8f381fb6c478130e5251eb3ab59c3d2669d859c8); /* statement */ 
g_priceInfo = PriceInfo({
            price: Math.to128(newPrice.value),
            lastUpdate: Time.currentTime()
        });

coverage_0x7dac2259(0x6c32b0c8075db018d2747a4104e1931be74f9b6e8ff35c34959daee6aad71cc3); /* line */ 
        coverage_0x7dac2259(0xd651686dbd930e3b15dee9a11533ee04495be2f27bb093c7ba12683ba88a6759); /* statement */ 
emit PriceSet(g_priceInfo);
coverage_0x7dac2259(0x0c3f9ca8fe401cb532307298b5d83e17d6fff4d65162fd04ea577d8eeae3bfa9); /* line */ 
        coverage_0x7dac2259(0xfad0c7c84e26be6f5a300641f63669a80d3adca1e4b74ca3eaa957a7d193ac83); /* statement */ 
return g_priceInfo;
    }

    // ============ IPriceOracle Functions ============

    function getPrice(
        address /* token */
    )
        public
        view
        returns (Monetary.Price memory)
    {coverage_0x7dac2259(0x67198145ec2e2ec394bcc25ea07f89a6d4bf2c6db9e2ea87d048f98a7d7ec960); /* function */ 

coverage_0x7dac2259(0xe5d417d813fd850e55cd47e25e1b8a00231f4172ed5a60411e6804947027f0b7); /* line */ 
        coverage_0x7dac2259(0x21612df4aae4f6932b00110a20c1cee818aedce49b7619c1ccbbb4878f82e856); /* statement */ 
return Monetary.Price({
            value: g_priceInfo.price
        });
    }

    // ============ Price-Query Functions ============

    /**
     * Get the new price that would be stored if updated right now.
     */
    function getBoundedTargetPrice()
        public
        view
        returns (Monetary.Price memory)
    {coverage_0x7dac2259(0x3d7bab3c5748245e289d2e24597f41e33614a0bd3760b1386a0381b6f5180419); /* function */ 

coverage_0x7dac2259(0x00135cb888da99d6a9bb90379b7977b386f8205b043338dfa1616b5b8dff516a); /* line */ 
        coverage_0x7dac2259(0x92e064e81027044eace325ca76f388ef31dc69e543fe17eeff7db87bfb0867db); /* statement */ 
Monetary.Price memory targetPrice = getTargetPrice();

coverage_0x7dac2259(0x67c768dfcedc60712a8482ae7367f7673d187ad7786c0f801deedbea171e3542); /* line */ 
        coverage_0x7dac2259(0x4cade384ef1072405f18e930f3c96d6fa613e4889d2e9047e1bc5688a3dc6623); /* statement */ 
PriceInfo memory oldInfo = g_priceInfo;
coverage_0x7dac2259(0xb0270eec02bde42d9ce1a9933cf5a95772ae29b17ae261440dc361a0374c0261); /* line */ 
        coverage_0x7dac2259(0xbad8502280feb24dd0e86bc846a49cadb89e32a6cc8d732647397aea3d964ace); /* statement */ 
uint256 timeDelta = uint256(Time.currentTime()).sub(oldInfo.lastUpdate);
coverage_0x7dac2259(0xc98b6cb730832812a1c27cc8009d69dd6f7d93d1a7b72f6fa864c4ed1bbc5be9); /* line */ 
        coverage_0x7dac2259(0x79a31ec64bdeca075cde2e59f958df25d6335f03c1011d1200bf76a2bf6cdb8f); /* statement */ 
(uint256 minPrice, uint256 maxPrice) = getPriceBounds(oldInfo.price, timeDelta);
coverage_0x7dac2259(0x5e85d4625273822709e8500bb367c8e78423311d2dc8700b58be4bf9839f582d); /* line */ 
        coverage_0x7dac2259(0xb1fa04873ca14d437d2d87b0d89a08cd6413918cf18f93e35ab1465a634cc91e); /* statement */ 
uint256 boundedTargetPrice = boundValue(targetPrice.value, minPrice, maxPrice);

coverage_0x7dac2259(0xbb3fb3181e277cefa66893d30b0cba51adf50d0ace0dda3817d741b471b186a8); /* line */ 
        coverage_0x7dac2259(0x4f4c353055c99e098092314703fc809e04e7c5db2a0b7ae897f5e2b2bde71899); /* statement */ 
return Monetary.Price({
            value: boundedTargetPrice
        });
    }

    /**
     * Get the DAI-USDC price that this contract will move towards when updated. This price is
     * not bounded by the variables governing the maximum deviation from the old price.
     */
    function getTargetPrice()
        public
        view
        returns (Monetary.Price memory)
    {coverage_0x7dac2259(0x707e1762f792c69220ca8b0aae0d94717f52a0356857b26e1ea2d089924d603b); /* function */ 

coverage_0x7dac2259(0xcc2b885b5f45573182d5052e350a223ddea9f82d9dc1916d2d523d1397c89864); /* line */ 
        coverage_0x7dac2259(0x0a486fa9093f5fbfef9362e970741236cf65f6ad66f6909ced9835223ac4cf23); /* statement */ 
uint256 targetPrice = getMidValue(
            EXPECTED_PRICE,
            getCurvePrice(),
            getUniswapPrice()
        );

coverage_0x7dac2259(0x5de7c1bcaa7ee05b09a776731b17a6845912a0c8b7faea1178927db96ca11eb7); /* line */ 
        coverage_0x7dac2259(0xc1123bdd953310988fce5e49b63d43af9673ef5b9990b2ff026d08c541aa3995); /* statement */ 
return Monetary.Price({
            value: targetPrice
        });
    }

    /**
     * Get the DAI-USDC price according to Curve.
     *
     * @return  The DAI-USDC price in natural units as a fixed-point number with 18 decimals.
     */
    function getCurvePrice()
        public
        view
        returns (uint256)
    {coverage_0x7dac2259(0x9b3f1c1d3d7b65be81bb3f0abcaf6a3be8888a847891623d0ab1065381ed985a); /* function */ 

coverage_0x7dac2259(0x68f27a4dac1c78ecd9a93603f5ac7707084c634ce2cbd1a9a5291e0c639e604f); /* line */ 
        coverage_0x7dac2259(0x0bbfaa67b4da84393254a2bb4cce3bb105f2b966cf53b061a7641467a34e1cfd); /* statement */ 
ICurve curve = CURVE;

        // Get dy when dx = 1, i.e. the number of DAI base units we can buy for 1 USDC base unit.
        //
        // After accounting for the fee, this is a very good estimate of the spot price.
coverage_0x7dac2259(0x28582c21daab738cbdec8763b1a30e156657bffeb731342ed56acca1170f03df); /* line */ 
        coverage_0x7dac2259(0x7f6b7e4fa3055e0350e65877dfabb12ba1a2afa716b879aa49a9da6aef6f0c2d); /* statement */ 
uint256 dyWithFee = curve.get_dy(CURVE_USDC_ID, CURVE_DAI_ID, 1);
coverage_0x7dac2259(0x37c75c7a4bf4c10acdae6bd85bd52c62c31326857bfa63653cb033953e33653f); /* line */ 
        coverage_0x7dac2259(0xe4b46ca26cd873116b4d2b80e8a4cf44a0758d66f80605611332e6def0afe1cd); /* statement */ 
uint256 fee = curve.fee();
coverage_0x7dac2259(0xb5179d93cb5fc85d710b3457df279e1cce3ce731ad6b64227b78a59eb1bc8237); /* line */ 
        coverage_0x7dac2259(0x501e87ae98d695a3da0781b69cea01cfccbc21bae9b951b49b41de2198cabd06); /* statement */ 
uint256 dyWithoutFee = dyWithFee.mul(CURVE_FEE_DENOMINATOR).div(
            CURVE_FEE_DENOMINATOR.sub(fee)
        );

        // Note that dy is on the order of 10^12 due to the difference in DAI and USDC base units.
        // We divide 10^30 by dy to get the price of DAI in USDC with 18 decimals of precision.
coverage_0x7dac2259(0x0b200ff756bee4805717a158dc72f391eb69ab8bda341753e06db17429225ebc); /* line */ 
        coverage_0x7dac2259(0xe2abf7d08b0ef32c1325ac9bafc5154a50c0145c1be0c482e808293d4c52910e); /* statement */ 
return CURVE_DECIMALS_BASE.div(dyWithoutFee);
    }

    /**
     * Get the DAI-USDC price according to Uniswap.
     *
     * @return  The DAI-USDC price in natural units as a fixed-point number with 18 decimals.
     */
    function getUniswapPrice()
        public
        view
        returns (uint256)
    {coverage_0x7dac2259(0xc9e25197f606a4c2c1967a5a170adb8e871b29ce8adcc934beab3599c2a3c985); /* function */ 

        // Note: Depending on the pool used, ETH may be the first asset or the second asset.
coverage_0x7dac2259(0x52f03788a14b8d2bfc0ee3e883b632a814c46bd07c8aa0bfec023f318113f53c); /* line */ 
        coverage_0x7dac2259(0x15c999895fc1098a9235280ae28cc10d13d1ba6df01de616bc980977b3884481); /* statement */ 
(uint256 daiAmt, uint256 poolOneEthAmt, ) = UNISWAP_DAI_ETH.getReserves();
coverage_0x7dac2259(0xd57c80dd3f7c12a7182e64ab657fe6e5d19cc961de4ff15b92c1f21d0968904a); /* line */ 
        coverage_0x7dac2259(0x2dddef9295e17b1787fc945fc2a50b0b97ed9d6864db909b891e09727ba87a78); /* statement */ 
(uint256 usdcAmt, uint256 poolTwoEthAmt, ) = UNISWAP_USDC_ETH.getReserves();

        // Get the price of DAI in USDC. Multiply by 10^30 to account for the difference in decimals
        // between DAI and USDC, and get a result with 18 decimals of precision.
        //
        // Note: There is a risk for overflow depending on the assets used and size of the pools.
coverage_0x7dac2259(0x743edd4a4908efb3710476dbc856c2606c9910125abdc65598385b3e36907ef1); /* line */ 
        coverage_0x7dac2259(0x1ad51e7f48d50f719b858fff0b8d4b234780d839932d0e1679bd12f32712c2f6); /* statement */ 
return UNISWAP_DECIMALS_BASE
            .mul(usdcAmt)
            .mul(poolOneEthAmt)
            .div(poolTwoEthAmt)
            .div(daiAmt);
    }

    // ============ Helper Functions ============

    function getPriceBounds(
        uint256 oldPrice,
        uint256 timeDelta
    )
        private
        view
        returns (uint256, uint256)
    {coverage_0x7dac2259(0x8cfdbfaca79980f514bb9af17c364a798f0c9022ae001bdf84dec6a0f01c7412); /* function */ 

coverage_0x7dac2259(0xbe6f5620c5f2537f7413c704b8903bc7872fd448418625f96ac542350d592bc1); /* line */ 
        coverage_0x7dac2259(0xdf8b709fdf06e58441aaad393910b45f6f0c51da1b0ef4fd26ac52f2a7d7a414); /* statement */ 
DeviationParams memory deviation = DEVIATION_PARAMS;

coverage_0x7dac2259(0x38e3ca7ae55658508e64c0a4fa169d7e4438c5ac0ee69cd34d7b77046c46e0aa); /* line */ 
        coverage_0x7dac2259(0x189770d54ab8339d5b715c395f76c05e79505e911726f5171bb27b33a343baa6); /* statement */ 
uint256 maxDeviation = Math.getPartial(
            oldPrice,
            Math.min(deviation.maximumAbsolute, timeDelta.mul(deviation.maximumPerSecond)),
            deviation.denominator
        );

coverage_0x7dac2259(0xc24ee82d4a570b4820be070315ebfd4f26191ab477258125cdf9f3d7844edfc6); /* line */ 
        coverage_0x7dac2259(0x487535020f76b12b7ccf2f9d27faf57a4cf8fdd9aedf4ce8994a18cb74a55cdc); /* statement */ 
return (
            oldPrice.sub(maxDeviation),
            oldPrice.add(maxDeviation)
        );
    }

    function getMidValue(
        uint256 valueA,
        uint256 valueB,
        uint256 valueC
    )
        private
        pure
        returns (uint256)
    {coverage_0x7dac2259(0x0c2020a258ecf7165f935d2c39551a73ee3680bdc9476810e33b3ac5458f0b0e); /* function */ 

coverage_0x7dac2259(0xb16a827e6409e2762c31b4a4952b16f9d5d2956509630b067dd4298d6273aff5); /* line */ 
        coverage_0x7dac2259(0x4f195997c4569b240ba70a3f8ace7cfcf63c8addb8db63e6a5c0cf602306128d); /* statement */ 
uint256 maximum = Math.max(valueA, Math.max(valueB, valueC));
coverage_0x7dac2259(0x13d9d948755f6603065d191788b30e3b20718cb8f7e1a7d6df5f51ce14fd699e); /* line */ 
        coverage_0x7dac2259(0x678f6acb18ee4ca736fc5746dd72916996b529d429f3c841a6ddfcc6984ed075); /* statement */ 
if (maximum == valueA) {coverage_0x7dac2259(0x29a7813a1a18fa66e438583ed07c45fe90d8be453e5717a06f4f53af40a8db5e); /* branch */ 

coverage_0x7dac2259(0x50a922f3bb958d5b311ff6c34d463c2bbe6874c831f6380d1d1f5b3c636fc8ff); /* line */ 
            coverage_0x7dac2259(0x856877ac64eea4b369c532a91182b3b53118d5ced0a6d1e5e77ad27334426ad8); /* statement */ 
return Math.max(valueB, valueC);
        }else { coverage_0x7dac2259(0x6cbe02956e26483bcf5f5dead3d8cfc2b8caf8a486e4b62d903cb5fa3b7fda6e); /* branch */ 
}
coverage_0x7dac2259(0x14cffe64f9d74973e8dc0dfb21de9438d477e8538c47f8be9445d4bc48df7d1c); /* line */ 
        coverage_0x7dac2259(0x972a4ff129f1352aeee02c07939a6dbf3100d78353842137fb7a05100268953b); /* statement */ 
if (maximum == valueB) {coverage_0x7dac2259(0x240a5003ab07a70ed642b8bca6699f4b67bd99f3f5954425249760d55f105a9f); /* branch */ 

coverage_0x7dac2259(0x69370ea77302c4da67f53da169ea0dd5a62d172746088c0379700caba6f600ae); /* line */ 
            coverage_0x7dac2259(0x70e8945e0cd02cc72df160c6a2a880139f36bff52f7e8938974a4ce4293f64dd); /* statement */ 
return Math.max(valueA, valueC);
        }else { coverage_0x7dac2259(0x5c94e33852f8f6a2bdaf6b6dd4513c1dc153d937b82372a169a11134261a39e8); /* branch */ 
}
coverage_0x7dac2259(0x42e84ffe35192e234e7bac7f199d44b386621be0cac71bce3d5e97fc561fc302); /* line */ 
        coverage_0x7dac2259(0xd3e9c571529c2a812e2adadae43e514327e0f385a56c4cfae669b5aaadf1d5c3); /* statement */ 
return Math.max(valueA, valueB);
    }

    function boundValue(
        uint256 value,
        uint256 minimum,
        uint256 maximum
    )
        private
        pure
        returns (uint256)
    {coverage_0x7dac2259(0x6dbee179a9c9f166f7a52408b0968fcb9635d23476c0433cf5eabeadec1940ce); /* function */ 

coverage_0x7dac2259(0xfae4a627ed7fdf0efc1a8ba85e31d1f265f5b556003db3df1e4e211d69c2f9a3); /* line */ 
        coverage_0x7dac2259(0x9216f32328d3da2e0dc19f62f7d19f2d48b8d3624fa58b5b88a8b10a44011823); /* assertPre */ 
coverage_0x7dac2259(0x9182eced4b019899c213e1bd03ef15640fae3a46531f41c71a1d87b8fdb5f4a3); /* statement */ 
assert(minimum <= maximum);coverage_0x7dac2259(0xd06b9aed498df15cc2059de28e9c4de2d4b68d998175c51eef9aa2169743f329); /* assertPost */ 

coverage_0x7dac2259(0x2674fcc4e5d89dd21cc839be2cfd8641cc3baeda2dba99e54c53cb87f9d3cc48); /* line */ 
        coverage_0x7dac2259(0x2b1044c381f1baaeb0409d576a6e5d4492ca8c04cb97070a58e196c7a8cb8f2b); /* statement */ 
return Math.max(minimum, Math.min(maximum, value));
    }
}
