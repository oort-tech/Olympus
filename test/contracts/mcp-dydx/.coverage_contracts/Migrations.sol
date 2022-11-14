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


contract Migrations {
function coverage_0x36f4edb7(bytes32 c__0x36f4edb7) public pure {}

    address public owner;
    uint256 public last_completed_migration;

    modifier restricted() {coverage_0x36f4edb7(0x08c0302bd4e832d3bd13dd1826b154280200d5d70618f1e166f8e1ba52ed163a); /* function */ 

coverage_0x36f4edb7(0xff82830dbe8584b668fb278009937fa7a6a6698fff65ea4fa1d047bd0fa39c3d); /* line */ 
        coverage_0x36f4edb7(0xd2ac7b43eb2ad8e857a93ff8d256bd41f1c615d3ad7eb5b6f5e0ea3500140d0b); /* statement */ 
if (msg.sender == owner) {coverage_0x36f4edb7(0x2123f0241d576c26a752bece4610fe5a8a5797106974fb4c456d8fa05fca8010); /* branch */ 

coverage_0x36f4edb7(0x070f1cb14488bb2ea6b622f6c9b89a3ef1f80b1cf16f82b6a610f435fe3c6fae); /* line */ 
            _;
        }else { coverage_0x36f4edb7(0xeb75a164db5a6b7da36eb8ba2d56ff1d3a1f53b3ccc3457bd5c93d7e85a4f9ba); /* branch */ 
}
    }

    constructor() public {coverage_0x36f4edb7(0x400a28d46f266d147b99f953f3fe8f4296dada11040b4cd563509a9661a6203c); /* function */ 

coverage_0x36f4edb7(0xe98a89e9629cfe471a7bcf0a1cb6eea51bade261ac28ae127bea3174d15b73cc); /* line */ 
        coverage_0x36f4edb7(0xb15756c900dee69fe947c01d47b9436d0acdd56707d8ccf91a98dd5da8d3761c); /* statement */ 
owner = msg.sender;
    }

    function setCompleted(uint256 completed) public restricted {coverage_0x36f4edb7(0xb28b037c7bde13ed9af952b583991519994bdf1c4e88d84b508b56656d7d34e6); /* function */ 

coverage_0x36f4edb7(0x6953c2659ffc1f2a5bf544de61fbd9197e3807437f3b79a9ba783bf06267248d); /* line */ 
        coverage_0x36f4edb7(0x9c5c68f30532e41ac16e1a8946dba95bab270a451cd4c83e38de3f03d8caa53d); /* statement */ 
last_completed_migration = completed;
    }

    function upgrade(address newAddress) public restricted {coverage_0x36f4edb7(0x55be192d338b877c99e69ac1473addd2c44c699ffcc294fb31a955fb91b8795b); /* function */ 

coverage_0x36f4edb7(0x233404999198f0da6c9be79f533eefd4f959f361438e96daac53afc7b994e3fb); /* line */ 
        coverage_0x36f4edb7(0x50b95930356ef5a8463574049c505501fb755bedd6410af9aa4e2cb4cc6f6d39); /* statement */ 
Migrations upgraded = Migrations(newAddress);
coverage_0x36f4edb7(0x12e22c802a56cc4b5e83809693f8d2c005a2caee7978c4dc811e203530e667b3); /* line */ 
        coverage_0x36f4edb7(0x5f1a58863411cb7b8fbc60386bd0f178a4238c55d6db2063e3644d325523f6fa); /* statement */ 
upgraded.setCompleted(last_completed_migration);
    }
}
