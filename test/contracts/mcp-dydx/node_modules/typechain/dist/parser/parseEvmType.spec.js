"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const chai_1 = require("chai");
const typeParser_1 = require("./typeParser");
describe("parseEvmType function", () => {
    it("should parse unsigned integer", () => {
        const parsedType = typeParser_1.parseEvmType("uint8");
        chai_1.expect(parsedType).to.be.instanceOf(typeParser_1.UnsignedIntegerType);
        chai_1.expect(parsedType.bits).to.be.eq(8);
    });
    it("should parse signed integer", () => {
        const parsedType = typeParser_1.parseEvmType("int");
        chai_1.expect(parsedType).to.be.instanceOf(typeParser_1.IntegerType);
        chai_1.expect(parsedType.bits).to.be.eq(256);
    });
    it("should parse boolean", () => {
        const parsedType = typeParser_1.parseEvmType("bool");
        chai_1.expect(parsedType).to.be.instanceOf(typeParser_1.BooleanType);
    });
    it("should parse bytes2", () => {
        const parsedType = typeParser_1.parseEvmType("bytes2");
        chai_1.expect(parsedType).to.be.instanceOf(typeParser_1.BytesType);
        chai_1.expect(parsedType.size).to.be.eq(2);
    });
    it("should parse arrays", () => {
        const parsedType = typeParser_1.parseEvmType("uint[]");
        chai_1.expect(parsedType).to.be.instanceOf(typeParser_1.ArrayType);
        chai_1.expect(parsedType.itemType).to.be.instanceOf(typeParser_1.UnsignedIntegerType);
    });
    it("should parse fixed size arrays", () => {
        const parsedType = typeParser_1.parseEvmType("uint[8]");
        chai_1.expect(parsedType).to.be.instanceOf(typeParser_1.ArrayType);
        chai_1.expect(parsedType.itemType).to.be.instanceOf(typeParser_1.UnsignedIntegerType);
        chai_1.expect(parsedType.size).to.be.eq(8);
    });
    it("should parse nested arrays", () => {
        const parsedType = typeParser_1.parseEvmType("uint16[8][256]");
        chai_1.expect(parsedType).to.be.instanceOf(typeParser_1.ArrayType);
        chai_1.expect(parsedType.itemType).to.be.instanceOf(typeParser_1.ArrayType);
        chai_1.expect(parsedType.size).to.be.eq(256);
        chai_1.expect(parsedType.itemType.itemType).to.be.instanceOf(typeParser_1.UnsignedIntegerType);
        chai_1.expect(parsedType.itemType.size).to.be.eq(8);
        chai_1.expect(parsedType.itemType.itemType.bits).to.be.eq(16);
    });
});
