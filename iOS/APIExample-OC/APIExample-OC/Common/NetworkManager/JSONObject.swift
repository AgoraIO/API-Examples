//
//  JSONObject.swift
//  BreakoutRoom
//
//  Created by zhaoyongqiang on 2021/11/2.
//

import Foundation

class JSONObject {
    /// Convert dictionary to model
    static func toModel<T: Codable>(_ type: T.Type, value: Any?) -> T? {
        guard let value = value else { return nil }
        return toModel(type, value: value)
    }
    /// Convert dictionary to model
    static func toModel<T: Codable>(_ type: T.Type, value: Any) -> T? {
        guard let data = try? JSONSerialization.data(withJSONObject: value) else { return nil }
        let decoder = JSONDecoder()
        decoder.nonConformingFloatDecodingStrategy = .convertFromString(positiveInfinity: "+Infinity", negativeInfinity: "-Infinity", nan: "NaN")
        return try? decoder.decode(type, from: data)
    }
    /// Convert JSON string to model
    static func toModel<T: Codable>(_ type: T.Type, value: String?) -> T? {
        guard let value = value else { return nil }
        return toModel(type, value: value)
    }
    /// Convert JSON string to model
    static func toModel<T: Codable>(_ type: T.Type, value: String) -> T? {
        let decoder = JSONDecoder()
        decoder.nonConformingFloatDecodingStrategy = .convertFromString(positiveInfinity: "+Infinity", negativeInfinity: "-Infinity", nan: "NaN")
        guard let t = try? decoder.decode(T.self, from: value.data(using: .utf8)!) else { return nil }
        return t
    }
    /// Convert model to JSON string
    static func toJson<T: Codable>(_ model: T) -> [String: Any] {
        let jsonString = toJsonString(model) ?? ""
        return toDictionary(jsonString: jsonString)
    }
    /// Convert model to JSON array string
    static func toJsonArray<T: Codable>(_ model: T) -> [[String: Any]]? {
        let jsonString = toJsonString(model) ?? ""
        return toArray(jsonString: jsonString)
    }
    /// Convert model to JSON string
    static func toJsonString<T: Codable>(_ model: T) -> String? {
        let encoder = JSONEncoder()
        encoder.outputFormatting = .prettyPrinted
        guard let data = try? encoder.encode(model) else { return nil }
        return String(data: data, encoding: .utf8)
    }
    /// Convert JSON string to dictionary
    static func toDictionary(jsonString: String) -> [String: Any] {
        guard let jsonData = jsonString.data(using: .utf8) else { return [:] }
        guard let dict = try? JSONSerialization.jsonObject(with: jsonData, options: .mutableContainers), let result = dict as? [String: Any] else { return [:] }
        return result
    }
    /// Convert JSON string to dictionary
    static func toDictionary(jsonStr: String) -> [String: String] {
        guard let jsonData = jsonStr.data(using: .utf8) else { return [:] }
        guard let dict = try? JSONSerialization.jsonObject(with: jsonData, options: .mutableContainers), let result = dict as? [String: Any] else { return [:] }
        var data = [String: String]()
        for item in result {
            data[item.key] = "\(item.value)"
        }
        return data
    }
    /// Convert JSON string to array
    static func toArray(jsonString: String) -> [[String: Any]]? {
        guard let jsonData = jsonString.data(using: .utf8) else { return nil }
        guard let array = try? JSONSerialization.jsonObject(with: jsonData, options: .mutableContainers), let result = array as? [[String: Any]] else { return nil }
        return result
    }
    /// Convert dictionary to JSON string
    static func toJsonString(dict: [String: Any]?) -> String? {
        guard let dict = dict else { return nil }
        if (!JSONSerialization.isValidJSONObject(dict)) {
            print("Invalid string format!")
            return nil
        }
        guard let data = try? JSONSerialization.data(withJSONObject: dict, options: []) else { return nil }
        guard let jsonString = String(data: data, encoding: .utf8) else { return nil }
        return jsonString
    }
    /// Convert dictionary array to JSON string
    static func toJsonString(array: [[String: Any]?]?) -> String? {
        guard let array = array else { return nil }
        var jsonString = "["
        var i = 0
        let count = array.count
        for dict in array {
            guard let dict = dict else { return nil }
            if (!JSONSerialization.isValidJSONObject(dict)) {
                print("Invalid string format!")
                return nil
            }
            guard let data = try? JSONSerialization.data(withJSONObject: dict, options: []) else { return nil }
            guard let tmp = String(data: data, encoding: .utf8) else { return nil }
            jsonString.append(tmp)
            if i < count - 1 {
                jsonString.append(",")
            }
            i = i + 1
        }
        jsonString.append("]")
        return jsonString
    }
}

extension String {
    func toArray() -> [[String: Any]]? {
        JSONObject.toArray(jsonString: self)
    }
    func toDictionary() -> [String : String] {
       JSONObject.toDictionary(jsonStr: self)
    }
}
