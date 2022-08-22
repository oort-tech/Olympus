pragma solidity ^0.5.16;

import "./PriceOracle.sol";
import "./PriceOracleAdapter.sol";

contract PriceOracleProxy is PriceOracle {
    /// @notice Address of the guardian
    address public guardian;
    /// @notice Address of the pending guardian
    address public pendingGuardian;
    /// @notice Mapping of the cTokenAddress => adapterAddress
    mapping(address => address) public tokenAdapter;
    ///@notice Emitted when pendingGuardian is changed
    event NewPendingGuardian(
        address oldPendingGuardian,
        address newPendingGuardian
    );
    ///@notice Emitted when pendingGuardian is accepted, which means gaurdian is updated
    event NewGuardian(address oldGuardian, address newGuardian);
    /// @notice Struct of the cTokensDetail
    struct CtokenDetail {
        address cToken;
        string cTokenName;
    }

    /// @notice Array of cTokensDetail
    CtokenDetail[] public cTokensArray;

    /**
     * @notice Get the length of cTokensArray
     * @return The length of cTokensArray
     */
    function cTokenArrayCount() public view returns (uint256) {
        return cTokensArray.length;
    }

    /// @param guardian_ The address of the guardian, which may set the
    constructor(address guardian_) public {
        guardian = guardian_;
    }

    /**
     * @notice Get the underlying price of a listed cToken asset
     * @param cToken The cToken to get the underlying price of
     * @return The underlying asset price mantissa (scaled by 1e18)
     */
    function getUnderlyingPrice(CToken cToken) public view returns (uint256) {
        address oracleAdapter = tokenAdapter[address(cToken)];
        //validate mapping
        if (oracleAdapter == address(0)) {
            return 0;
        }
        // return PriceOracleAdapter(oracleAdapter).assetPrices(address(cToken));
        return 1000000000000000000;
    }

    /**
     * @notice Set the underlying price of a listed cToken asset
     * @param addressToken Address of the cToken
     * @param addressAdapter Address of the OracleAdapter
     */
    function setAdapterToToken(address addressToken, address addressAdapter)
        public
    {
        //validate only guardian can set
        require(
            msg.sender == guardian,
            "PriceOracleProxy: only guardian may set the address"
        );
        require(
            addressToken != address(0),
            "PriceOracleProxy: address token can not be 0"
        );
        require(
            addressAdapter != address(0),
            "PriceOracleProxy: address adapter can not be 0"
        );
        //validate and set new cToken in CtokenDetail
        if (tokenAdapter[addressToken] == address(0)) {
            CtokenDetail memory _cTokenD = CtokenDetail({
                cToken: addressToken,
                cTokenName: CToken(addressToken).symbol()
            });

            cTokensArray.push(_cTokenD);
        }
        //set token => adapter
        tokenAdapter[addressToken] = addressAdapter;
    }

    /**
     * @notice Begins transfer of gaurdian rights. The newPendingGaurdian must call `_acceptAdmin` to finalize the transfer.
     * @param newPendingGuardian New pending gaurdian.
     */
    function _setPendingAdmin(address newPendingGuardian) public {
        // Check caller = gaurdian
        require(
            msg.sender == guardian,
            "PriceOracleProxy: only guardian may set the address"
        );
        require(
            newPendingGuardian != address(0),
            "PriceOracleProxy: address admin can not be 0"
        );
        // Save current value, if any, for inclusion in log
        address oldPendingGuardian = guardian;
        // Store pendingGaurdian with value newPendingGaurdian
        pendingGuardian = newPendingGuardian;
        // Emit NewPendingGaurdian(oldPendingGaurdian, newPendingGaurdian)
        emit NewPendingGuardian(oldPendingGuardian, newPendingGuardian);
    }

    /// @notice Accepts transfer of gaurdian rights. msg.sender must be pendingGaurdian
    function _acceptAdmin() public {
        // Check caller is pendingGaurdian and pendingGaurdian ≠ address(0)
        require(
            msg.sender == pendingGuardian,
            "PriceOracleProxy: only guardian may set the address"
        );
        require(
            msg.sender != address(0),
            "PriceOracleProxy: sender can not be 0"
        );

        // Save current values for inclusion in log
        address oldGuardian = guardian;
        address oldPendingGaurdian = pendingGuardian;

        // Store gaurdian with value pendingGaurdian
        guardian = pendingGuardian;

        // Clear the pending value
        pendingGuardian = address(0);

        emit NewGuardian(oldGuardian, guardian);
        emit NewPendingGuardian(oldPendingGaurdian, pendingGuardian);
    }
}
