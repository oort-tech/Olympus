pragma solidity ^0.5.16;

import "./PriceOracleAdapter.sol";

interface PriceProviderMoC {
    // function peek() external view returns (bytes32, bool);
    function decimals() external view returns (uint8);
    function latestRoundData()
        external
        view
        returns (
          uint80 roundId,
          int256 answer,
          uint256 startedAt,
          uint256 updatedAt,
          uint80 answeredInRound
        );
}

interface UniswapV2Pair {
    function getReserves() external view returns (uint112 reserve0, uint112 reserve1, uint32 blockTimestampLast);
    function token0() external view returns(address);
    function token1() external view returns(address);
}
interface Erc20 {
    function decimals() external view returns (uint8);
}

contract PriceOracleAdapterMoc is PriceOracleAdapter {
    /// @notice Address of the guardian
    address public guardian;
    /// @notice The MoC price oracle, which will continue to serve prices
    PriceProviderMoC public priceProviderMoC;
    UniswapV2Pair public uniswapV2Pair;

    /// @notice The highest ratio of the new price to the anchor price that will still trigger the price to be updated
    uint public upperBoundAnchorRatio;

    /// @notice The lowest ratio of the new price to the anchor price that will still trigger the price to be updated
    uint public lowerBoundAnchorRatio;

    bool private isUniswapReversed;

    /// @notice Guardian updated
    event NewGuardian(address oldGuardian,address newGuardian);

    /**
     * @notice Construct a PriceOracleAdapter for a MoC oracle
     * @param guardian_ address of guardian that is allowed to manage this contract
     * @param priceProvider address of asset's MoC price provider
     */
    constructor(address guardian_,address priceProvider, address pair, bool reversed, uint anchorToleranceMantissa_) public {
        require(
            guardian_ != address(0),
            "PriceOracleAdapterMoc: guardian could not be 0"
        );
        require(
            priceProvider != address(0),
            "PriceOracleAdapterMoc: priceProvider could not be 0"
        );
        guardian = guardian_;
        priceProviderMoC = PriceProviderMoC(priceProvider);
        uniswapV2Pair = UniswapV2Pair(pair);
        isUniswapReversed = reversed;
        upperBoundAnchorRatio = anchorToleranceMantissa_ > uint(-1) - 100e16 ? uint(-1) : 100e16 + anchorToleranceMantissa_;
        lowerBoundAnchorRatio = anchorToleranceMantissa_ < 100e16 ? 100e16 - anchorToleranceMantissa_ : 1;
    }

    function getAnchorPrice() public view returns(uint256) {
        uint anchorPrice;
        (uint112 reserve0, uint112 reserve1, uint32 blockTimestampLast) = uniswapV2Pair.getReserves();
        uint8 d0 = Erc20(uniswapV2Pair.token0()).decimals();
        uint8 d1 = Erc20(uniswapV2Pair.token1()).decimals();
        if (isUniswapReversed) anchorPrice = uint256(reserve1) * 10 ** d1 * 10 ** 18 / (uint256(reserve0) * 10 ** d0);
        else anchorPrice = uint256(reserve0) * 10 ** d0 * 10 ** 18 / (uint256(reserve1) * 10 ** d1);
        return anchorPrice;
    }

    /**
     * @notice Get the price from MoC and divide it by the rBTC price
     * @return The price
     */
    function assetPrices(address) public view returns (uint256) {
        // (bytes32 price, bool has) = priceProviderMoC.peek();
        // require(has, "PriceOracleAdapterMoc: Oracle have no Price");
        // return uint256(price);
        
        (, int256 _price, , , ) = priceProviderMoC.latestRoundData();
        uint8 _decimals = priceProviderMoC.decimals();
        uint reporterPrice = uint256(_price) * (uint256(10) ** (18 - _decimals));
        uint anchorPrice = getAnchorPrice();
        require (isWithinAnchor(reporterPrice, anchorPrice), 'Reported price is out of bounds.');
        return reporterPrice;
    }

    function isWithinAnchor(uint reporterPrice, uint anchorPrice) internal view returns (bool) {
        if (reporterPrice > 0) {
            uint anchorRatio = anchorPrice * 100e16 / reporterPrice;
            return anchorRatio <= upperBoundAnchorRatio && anchorRatio >= lowerBoundAnchorRatio;
        }
        return false;
    }

    /**
     * @notice Set the address of price provider
     * @param priceProviderAddress address of price provider
     */
    function setPriceProvider(address priceProviderAddress) public {
        require(
            msg.sender == guardian,
            "PriceOracleAdapterMoc: only guardian may set the address"
        );
        require(
            priceProviderAddress != address(0),
            "PriceOracleAdapterMoc: address could not be 0"
        );
        //set old address
        address oldPriceProviderAddress = address(priceProviderMoC);
        //update interface address
        priceProviderMoC = PriceProviderMoC(priceProviderAddress);
        //emit event
        emit PriceOracleAdapterUpdated(
            oldPriceProviderAddress,
            priceProviderAddress
        );
    }

    /**
     * @notice Set the address of the guardian
     * @param newGuardian address of the guardian
     */
    function setGuardian(address newGuardian) public {
        require(
            msg.sender == guardian,
            "PriceOracleAdapterMoc: only guardian"
        );
        require(
            guardian != address(0),
            "PriceOracleAdapterMoc: guardin address can not be 0"
        );
        //set old address
        address oldGuardian = guardian;
        //update
        guardian = newGuardian;
        //emit event
        emit NewGuardian(
            oldGuardian,
            newGuardian
        );
    }
}
