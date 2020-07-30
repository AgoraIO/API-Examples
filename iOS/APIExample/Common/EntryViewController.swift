//
//  WelcomeViewController.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/7/22.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Foundation
import UIKit

class EntryViewController : UIViewController
{
    @IBOutlet weak var joinButton: AGButton!
    @IBOutlet weak var channelTextField: AGTextField!
    @IBOutlet weak var noteLabel: UILabel!
    var note: String = ""
    
    //identifer of next view controller after press join button
    var nextVCIdentifier: String = ""
    
    override func viewDidLoad() {
        super.viewDidLoad()
        noteLabel.text = note
    }
    
    @IBAction func doJoinPressed(sender: AGButton) {
        guard let channelName = channelTextField.text else {return}
        //resign channel text field
        channelTextField.resignFirstResponder()
        
        let storyBoard: UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
        // create new view controller every time to ensure we get a clean vc
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: nextVCIdentifier) as? BaseViewController else {return}
        newViewController.title = channelName
        newViewController.configs = ["channelName":channelName]
        self.navigationController?.pushViewController(newViewController, animated: true)
    }
}
