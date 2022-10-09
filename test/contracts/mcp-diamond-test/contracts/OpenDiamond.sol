// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

import { LibDiamond } from "./libraries/LibDiamond.sol";
import { LibOracle } from "./libraries/LibOracle.sol";
import { IDiamondCut } from "./interfaces/IDiamondCut.sol";

contract OpenDiamond {
    constructor(address _upgradeAdmin, address _diamondCutFacet) payable {
        LibDiamond.setupgradeAdmin(_upgradeAdmin);
        // Add the diamondCut external function from the diamondCutFacet
        IDiamondCut.FacetCut[] memory cut = new IDiamondCut.FacetCut[](1);
        bytes4[] memory functionSelectors = new bytes4[](1);
        functionSelectors[0] = IDiamondCut.diamondCut.selector;
        cut[0] = IDiamondCut.FacetCut({
            facetAddress: _diamondCutFacet,
            action: IDiamondCut.FacetCutAction.Add,
            functionSelectors: functionSelectors,
            facetId: 0
        });
        LibDiamond.diamondCut(cut, address(0), "");
    }

    fallback() external payable {
        LibDiamond.DiamondStorage storage ds;
        bytes32 position = LibDiamond.DIAMOND_STORAGE_POSITION;
        // get diamond storage
        assembly {
            ds.slot := position
        }

        address facet = ds.facetAddressAndSelectorPosition[msg.sig].facetAddress;
        require(facet != address(0), "Diamond: Function does not exist");

        assembly {
            // copy function selector and any arguments
            calldatacopy(0, 0, calldatasize())
            // execute function call using the facet
            let result := delegatecall(gas(), facet, 0, calldatasize(), 0, 0)
            // get any return value
            returndatacopy(0, 0, returndatasize())
            // return any return value or error back to the caller
            switch result
            case 0 {
                revert(0, returndatasize())
            }
            default {
                return(0, returndatasize())
            }
        }
    }

    function addFairPriceAddress(bytes32 _market, address _address) external {
        LibOracle._addFairPriceAddress(_market, _address);
    }

    function getFairPriceAddress(bytes32 _market) external view returns (address) {
        return LibOracle._getFairPriceAddress(_market);
    }

    receive() external payable {}
}
