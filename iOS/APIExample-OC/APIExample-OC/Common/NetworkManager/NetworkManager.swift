//
//  NetworkManager.swift
//  Scene-Examples
//
//  Created by zhaoyongqiang on 2021/11/19.
//

import UIKit

@objc
class NetworkManager: NSObject {
    enum HTTPMethods: String {
        case GET = "GET"
        case POST = "POST"
    }
        
    typealias SuccessClosure = ([String: Any]) -> Void
    typealias FailClosure = (String) -> Void
    
    private lazy var sessionConfig: URLSessionConfiguration = {
        let config = URLSessionConfiguration.default
        config.httpAdditionalHeaders = ["Content-Type": "application/json",
                                        "X-LC-Id": "fkUjxadPMmvYF3F3BI4uvmjo-gzGzoHsz",
                                        "X-LC-Key": "QAvFS62IOR28GfSFQO5ze45s",
                                        "X-LC-Session": "qmdj8pdidnmyzp0c7yqil91oc"]
        config.timeoutIntervalForRequest = 30
        config.timeoutIntervalForResource = 30
        config.requestCachePolicy = .reloadIgnoringLocalCacheData
        return config
    }()
    
    @objc
    static let shared = NetworkManager()
    private override init() { }
    private let baseUrl = "https://service.agora.io/toolbox-global/v1/token/generate"
    
    @objc
    func generateToken(channelName: String, uid: UInt = 0, success: @escaping (String?) -> Void) {
        if KeyCenter.certificate() == nil || KeyCenter.certificate()?.isEmpty == true {
            success(nil)
            return
        }
        let params = ["appCertificate": KeyCenter.certificate() ?? "",
                      "appId": KeyCenter.appId() ?? "",
                      "channelName": channelName,
                      "expire": 900,
                      "src": "iOS",
                      "ts": "".timeStamp,
                      "type": 1,
                      "uid": "\(uid)"] as [String : Any]
        ToastView.showWait(text: "loading...", view: nil)
        NetworkManager.shared.postRequest(urlString: "https://toolbox.bj2.agoralab.co/v1/token/generate", params: params, success: { response in
            let data = response["data"] as? [String: String]
            let token = data?["token"]
            print(response)
            success(token)
            ToastView.hidden()
        }, failure: { error in
            print(error)
            success(nil)
            ToastView.hidden()
        })
    }
    
    func getRequest(urlString: String, success: SuccessClosure?, failure: FailClosure?) {
        DispatchQueue.global().async {
            self.request(urlString: urlString, params: nil, method: .GET, success: success, failure: failure)
        }
    }
    
    func download(urlString: String, success: SuccessClosure?, failure: FailClosure?) {
        let session = URLSession(configuration: sessionConfig)
        let request = URLRequest(url: URL(string: urlString) ?? URL(fileURLWithPath: urlString))
        let fileName = urlString.components(separatedBy: "/").last ?? ""
        let documnets = NSTemporaryDirectory() + fileName
        if FileManager.default.fileExists(atPath: documnets) {
            success?(["fileName": fileName, "path": documnets])
            return
        }
        DispatchQueue.global().async {
            let downloadTask = session.downloadTask(with: request) { location, response, error in
                let locationPath = location!.path
                let fileManager = FileManager.default
                try? fileManager.moveItem(atPath: locationPath, toPath: documnets)
                success?(["fileName": fileName, "path": documnets])
            }
            downloadTask.resume()
        }
    }
    
    func postRequest(urlString: String, params: [String: Any]?, success: SuccessClosure?, failure: FailClosure?) {
        DispatchQueue.global().async {
            self.request(urlString: urlString, params: params, method: .POST, success: success, failure: failure)
        }
    }
    
    private func request(urlString: String,
                         params: [String: Any]?,
                         method: HTTPMethods,
                         success: SuccessClosure?,
                         failure: FailClosure?) {
        let session = URLSession(configuration: sessionConfig)
        guard let request = getRequest(urlString: urlString,
                                       params: params,
                                       method: method,
                                       success: success,
                                       failure: failure) else { return }
        session.dataTask(with: request) { data, response, error in
            DispatchQueue.main.async {
                self.checkResponse(response: response, data: data, success: success, failure: failure)
            }
        }.resume()
    }
    
    private func getRequest(urlString: String,
                            params: [String: Any]?,
                            method: HTTPMethods,
                            success: SuccessClosure?,
                            failure: FailClosure?) -> URLRequest? {
        
        let string = urlString.hasPrefix("http") ? urlString : baseUrl.appending(urlString)
        guard let url = URL(string: string) else {
            return nil
        }
        var request = URLRequest(url: url)
        request.httpMethod = method.rawValue
        if method == .POST {
            request.httpBody = try? JSONSerialization.data(withJSONObject: params ?? [], options: .sortedKeys)//convertParams(params: params).data(using: .utf8)
        }
        let curl = request.cURL(pretty: true)
        debugPrint("curl == \(curl)")
        return request
    }
    
    private func convertParams(params: [String: Any]?) -> String {
        guard let params = params else { return "" }
        let value = params.map({ String(format: "%@=%@", $0.key, "\($0.value)") }).joined(separator: "&")
        return value
    }
    
    private func checkResponse(response: URLResponse?, data: Data?, success: SuccessClosure?, failure: FailClosure?) {
        if let httpResponse = response as? HTTPURLResponse {
            switch httpResponse.statusCode {
            case 200...201:
                if let resultData = data {
                    let result = String(data: resultData, encoding: .utf8)
                    print(result ?? "")
                    success?(JSONObject.toDictionary(jsonString: result ?? ""))
                } else {
                    failure?("Error in the request status code \(httpResponse.statusCode), response: \(String(describing: response))")
                }
            default:
                failure?("Error in the request status code \(httpResponse.statusCode), response: \(String(describing: response))")
            }
        }
    }
}

extension URLRequest {
    public func cURL(pretty: Bool = false) -> String {
        let newLine = pretty ? "\\\n" : ""
        let method = (pretty ? "--request " : "-X ") + "\(httpMethod ?? "GET") \(newLine)"
        let url: String = (pretty ? "--url " : "") + "\'\(url?.absoluteString ?? "")\' \(newLine)"
        
        var cURL = "curl "
        var header = ""
        var data: String = ""
        
        if let httpHeaders = allHTTPHeaderFields, httpHeaders.keys.count > 0 {
            for (key,value) in httpHeaders {
                header += (pretty ? "--header " : "-H ") + "\'\(key): \(value)\' \(newLine)"
            }
        }
        
        if let bodyData = httpBody, let bodyString = String(data: bodyData, encoding: .utf8), !bodyString.isEmpty {
            data = "--data '\(bodyString)'"
        }
        
        cURL += method + url + header + data
        
        return cURL
    }
}

extension String {
    var timeStamp: String {
        let date = Date()
        let timeInterval = date.timeIntervalSince1970
        let millisecond = CLongLong(timeInterval * 1000)
        return "\(millisecond)"
    }
}
