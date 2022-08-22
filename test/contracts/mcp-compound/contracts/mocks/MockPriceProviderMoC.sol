pragma solidity ^0.5.16;

import "../CErc20.sol";

/**
 * @title A mock price provider of Money on Chain (MoC)
 * @notice You can use this contract for only simulation
 */
contract MockPriceProviderMoC {
    /// @notice rbtcPrice of the interface provicer MoC
    bytes32 rbtcPrice;
    /// @notice has of the interface provicer MoC
    bool has;
    /// @notice Address of the guardian
    address public guardian;
    /// @notice Event rbtcPrice updated
    event MockPriceProviderMoCUpdated(uint256 oldPrice, uint256 newPrice);

    constructor(address guardian_, uint256 price) public {
        require(
            guardian_ != address(0),
            "MockPriceProviderMoC: address could not be 0"
        );
        require(
            price != uint256(0),
            "MockPriceProviderMoC: price could not be 0"
        );
        guardian = guardian_;
        rbtcPrice = bytes32(price);
        has = true;
    }

    function peek() public view returns (bytes32, bool) {
        return (rbtcPrice, has);
    }

    /**
     * @notice Set the rbtcPrice price provider
     * @param price uint of price provider
     */
    function setPrice(uint256 price) public {
        require(
            msg.sender == guardian,
            "MockPriceProviderMoC: only guardian may set the address"
        );
        require(
            price != uint256(0),
            "MockPriceProviderMoC: price could not be 0"
        );
        //set old price
        bytes32 oldRbtcPrice = rbtcPrice;
        //update rbtcPrice
        rbtcPrice = bytes32(price);
        //emit event
        emit MockPriceProviderMoCUpdated(uint256(oldRbtcPrice), uint256(rbtcPrice));
    }
}
