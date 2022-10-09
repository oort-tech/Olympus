pragma solidity ^0.5.16;

import "../PriceOracleAdapter.sol";
import "../CErc20.sol";

interface V1PriceOracleInterface {
    function assetPrices(address asset) external view returns (uint256);
}

contract PriceOracleAdapterCompound is PriceOracleAdapter {
    /// @notice Address of the guardian
    address public guardian;
    /// @notice Event oracle key updateed
    event PriceOracleKeyUpdated(
        address oldAddress,
        address newAddress,
        address cTokenAddress
    );
    /// @notice The price oracle, which will continue to serve prices of compound
    V1PriceOracleInterface public priceProviderInterface;

    // mapping(addressCtoken => addressKeyOracle);
    mapping(address => address) public oracleKeyAddress;

    /// @notice Frozen SAI price (or 0 if not set yet)
    uint256 public saiPrice;

    constructor(address guardian_) public {
        guardian = guardian_;
    }

    /**
     * @notice Get the price
     * @param cTokenAddress address of cToken
     * @return The price
     */
    function assetPrices(address cTokenAddress) public view returns (uint256) {
        //get keyAddress or undlerlyingAddress
        address asset = (oracleKeyAddress[cTokenAddress] != address(0))
            ? address(oracleKeyAddress[cTokenAddress])
            : address(CErc20(cTokenAddress).underlying());
        return priceProviderInterface.assetPrices(asset);
    }

    /**
     * @notice Set the address of price provider
     * @param priceProviderAddress address of price provider
     */
    function setPriceProvider(address priceProviderAddress) public {
        require(
            msg.sender == guardian,
            "PriceOracleAdapterCompound: only guardian may set the address"
        );
        require(
            priceProviderAddress != address(0),
            "PriceOracleAdapterCompound: address could not be 0"
        );
        //set old address
        address oldBtcPriceProviderAddress = address(priceProviderInterface);
        //update interface address
        priceProviderInterface = V1PriceOracleInterface(priceProviderAddress);
        //emit event
        emit PriceOracleAdapterUpdated(
            oldBtcPriceProviderAddress,
            address(priceProviderInterface)
        );
    }

    /**
     * @notice Set the key oracle address of cToken address
     * @param cTokenAddress address of key ctoken
     * @param keyOracle address of key oracle
     */
    function setKeyOracle(address cTokenAddress, address keyOracle) public {
        require(
            msg.sender == guardian,
            "PriceOracleAdapterCompound: only guardian may set the address"
        );
        require(
            cTokenAddress != address(0),
            "PriceOracleAdapterCompound: cTokenAddress could not be 0"
        );
        require(
            keyOracle != address(0),
            "PriceOracleAdapterCompound: keyOracle could not be 0"
        );
        //set old address
        address oldBtcPriceProviderAddress = address(
            oracleKeyAddress[cTokenAddress]
        );
        //update key address
        oracleKeyAddress[cTokenAddress] = keyOracle;
        //emit event
        emit PriceOracleKeyUpdated(
            oldBtcPriceProviderAddress,
            address(oracleKeyAddress[cTokenAddress]),
            cTokenAddress
        );
    }

    /**
     * @notice Set the price of SAI, permanently
     * @param price The price for SAI
     */
    function setSaiPrice(uint256 price) public {
        require(msg.sender == guardian, "only guardian may set the SAI price");
        require(saiPrice == 0, "SAI price may only be set once");
        require(price < 0.1e18, "SAI price must be < 0.1 ETH");
        saiPrice = price;
    }
}
