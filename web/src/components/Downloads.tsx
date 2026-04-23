"use client";
import { useEffect, useState } from "react";
import ScrollReveal from "./ScrollReveal";

interface Asset {
    name: string;
    browser_download_url: string;
    size: number;
}

interface ReleaseData {
    version: string;
    assets: Asset[];
}

export default function Downloads() {
    const [release, setRelease] = useState<ReleaseData | null>(null);
    const [error, setError] = useState(false);

    useEffect(() => {
        const fetchReleases = async () => {
            try {
                const res = await fetch("https://api.github.com/repos/salem-5/StudySync/releases/latest");
                if (!res.ok) throw new Error("Failed to fetch");
                const data = await res.json();
                setRelease({ version: data.tag_name, assets: data.assets });
            } catch (err) {
                console.error(err);
                setError(true);
            }
        };
        fetchReleases();
    }, []);

    const getAssetDetails = (filename: string) => {
        if (error) return { text: "Error loading latest", url: "#", loading: false };
        if (!release) return { text: "Fetching metadata...", url: "#", loading: true };

        const asset = release.assets.find(a => a.name === filename);
        if (!asset) return { text: "File not found", url: "#", loading: false };

        return {
            text: `${release.version} • ${(asset.size / (1024 * 1024)).toFixed(2)} MB`,
            url: asset.browser_download_url,
            loading: false
        };
    };

    const clientAssets = [
        { id: 'studysync_client-linux-amd64', title: 'Linux (amd64)', icon: 'ph-linux-logo' },
        { id: 'studysync_client-linux-arm64', title: 'Linux (arm64)', icon: 'ph-linux-logo' },
        { id: 'studysync_client-macos-arm64', title: 'macOS (Apple Silicon)', icon: 'ph-apple-logo' }
    ];

    const serverAssets = [
        { id: 'studysync_server-linux-amd64', title: 'Linux (amd64)', icon: 'ph-linux-logo' },
        { id: 'studysync_server-linux-arm64', title: 'Linux (arm64)', icon: 'ph-linux-logo' },
        { id: 'studysync_server-macos-arm64', title: 'macOS (Apple Silicon)', icon: 'ph-apple-logo' }
    ];

    return (
        <section className="download-section" id="downloads">
            <div className="download-wrapper">
                <ScrollReveal className="dl-column">
                    <h2>Desktop Client</h2>
                    {clientAssets.map(asset => {
                        const details = getAssetDetails(asset.id);
                        return (
                            <div className="dl-card" key={asset.id}>
                                <div className="dl-meta">
                                    <span className="dl-title"><i className={`ph-fill ${asset.icon}`}></i> {asset.title}</span>
                                    <span>{details.text}</span>
                                </div>
                                <a href={details.url} className={`dl-action-btn ${details.loading ? 'loading' : ''}`}>
                                    <i className="ph-bold ph-download-simple"></i> Download
                                </a>
                            </div>
                        );
                    })}
                </ScrollReveal>

                <ScrollReveal className="dl-column">
                    <h2>Server Backend</h2>
                    {serverAssets.map(asset => {
                        const details = getAssetDetails(asset.id);
                        return (
                            <div className="dl-card" key={asset.id}>
                                <div className="dl-meta">
                                    <span className="dl-title"><i className={`ph-fill ${asset.icon}`}></i> {asset.title}</span>
                                    <span>{details.text}</span>
                                </div>
                                <a href={details.url} className={`dl-action-btn ${details.loading ? 'loading' : ''}`}>
                                    <i className="ph-bold ph-download-simple"></i> Download
                                </a>
                            </div>
                        );
                    })}
                </ScrollReveal>
            </div>
        </section>
    );
}