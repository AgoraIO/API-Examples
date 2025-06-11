//
//  HomeViewController.swift
//  PIPDemo
//
//  Created by qinhui on 2024/8/8.
//

import UIKit

class Model<T: PIPBaseViewController> {
    var title: String
    var cls: T.Type
    init(title: String, cls: T.Type) {
        self.title = title
        self.cls = cls
    }
}

class PictureInPicture: UITableViewController {
    lazy var dataArray: [Model<PIPBaseViewController>] = {
        if #available(iOS 15.0, *) {
            return [
                Model<PIPBaseViewController>(title: "多人视频自渲染", cls: PixelBufferPIPViewController.self)
            ]
        } else {
            // Fallback on earlier versions
            return []
        }
    }()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        title = "Picture In Picture"
        tableView.register(UITableViewCell.self, forCellReuseIdentifier: "cell")
    }
    
    override func tableView(_ tableView: UITableView, heightForRowAt indexPath: IndexPath) -> CGFloat {
        return 50
    }
    
    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        dataArray.count
    }
    
    override func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        let model = dataArray[indexPath.row]
        let vc = ChannelViewController()
        vc.pipCls = model.cls
        self.navigationController?.pushViewController(vc, animated: true)
    }
    
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "cell", for: indexPath)
        let model = dataArray[indexPath.row]
        cell.textLabel?.text = model.title
        return cell
    }
}
